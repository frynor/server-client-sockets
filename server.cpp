#include "network.h"
#include <unistd.h>
#include <algorithm>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <string.h>
#include <errno.h>

Server::Server() : serverSocket(-1) {}

Server::~Server() {
    stop();
}

bool Server::createSocket() {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Can not create a socket: " << strerror(errno) << std::endl;
        return false;
    }
    return true;
}

bool Server::bindSocket() {
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(SERVER_PORT);

    if (bind(serverSocket, (struct sockaddr *)&address, sizeof(address)) == -1) {
        std::cerr << "Can not bind to the IP/Port: " << strerror(errno) << std::endl;
        return false;
    }
    return true;
}

bool Server::listenForConnections(int backlog) {
    if (listen(serverSocket, backlog) == -1) {
        std::cerr << "Problem with listening: " << strerror(errno) << std::endl;
        return false;
    }
    std::cout << "Server is listening" << std::endl;
    return true;
}

int Server::acceptClient() {
    struct sockaddr_in clientAddr;
    socklen_t clientSize = sizeof(clientAddr);
    int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientSize);
    if (clientSocket == -1) {
        std::cerr << "There is a connection issue: " << strerror(errno) << std::endl;
    } else {
        clientSockets.push_back(clientSocket);
    }
    return clientSocket;
}

void Server::handleClient(int clientSocket) {
    char buffer[1024];
    memset(buffer, 0, 1024);
    int byteReceived = recv(clientSocket, buffer, 1024, 0);
    if (byteReceived == -1) {
        std::cerr << "Recv() failed: " << strerror(errno) << std::endl;
        closeClientSocket(clientSocket);
    } else if (byteReceived == 0) {
        std::cout << "Client disconnected" << std::endl;
        closeClientSocket(clientSocket);
    } else {
        std::cout << "Received from client " << clientSocket << ": " << std::string(buffer, 0, byteReceived) << std::endl;
        send(clientSocket, buffer, byteReceived, 0);
    }
}

void Server::run() {
    fd_set readfds;
    int max_sd;

    while (true) {
        FD_ZERO(&readfds);
        FD_SET(serverSocket, &readfds);
        max_sd = serverSocket;

        for (int clientSocket : clientSockets) {
            if (clientSocket > 0) {
                FD_SET(clientSocket, &readfds);
            }
            if (clientSocket > max_sd) {
                max_sd = clientSocket;
            }
        }

        struct timeval timeout;
        timeout.tv_sec = 1;  // 1 second timeout
        timeout.tv_usec = 0;

        int activity = select(max_sd + 1, &readfds, NULL, NULL, &timeout);
        
        if (activity < 0 && errno != EINTR) {
            std::cerr << "select error: " << strerror(errno) << std::endl;
            continue;
        }

        if (FD_ISSET(serverSocket, &readfds)) {
            int newSocket = acceptClient();
            if (newSocket != -1) {
                std::cout << "New connection, socket fd is " << newSocket << std::endl;
            }
        }

        for (auto it = clientSockets.begin(); it != clientSockets.end();) {
            int clientSocket = *it;
            if (FD_ISSET(clientSocket, &readfds)) {
                char buffer[1024] = {0};
                int bytesReceived = recv(clientSocket, buffer, 1024, 0);
                if (bytesReceived <= 0) {
                    if (bytesReceived == 0) {
                        std::cout << "Client disconnected, socket fd " << clientSocket << std::endl;
                    } else {
                        std::cerr << "recv() failed for socket " << clientSocket << ": " << strerror(errno) << std::endl;
                    }
                    closeClientSocket(clientSocket);
                    it = clientSockets.erase(it);
                } else {
                    std::cout << "Received from client " << clientSocket << ": " << std::string(buffer, bytesReceived) << std::endl;
                    send(clientSocket, buffer, bytesReceived, 0);
                    ++it;
                }
            } else {
                ++it;
            }
        }
    }
}

void Server::closeClientSocket(int clientSocket) {
    close(clientSocket);
    std::cout << "Closed connection on socket " << clientSocket << std::endl;
}

// New implementations for missing methods

bool Server::start() {
    if (!createSocket()) {
        return false;
    }
    if (!bindSocket()) {
        return false;
    }
    if (!listenForConnections()) {
        return false;
    }

    std::cout << "Server started successfully. Listening on port " << SERVER_PORT << std::endl;
    return true;
}

void Server::stop() {
    for (int clientSocket : clientSockets) {
        closeClientSocket(clientSocket);
    }
    clientSockets.clear();

    if (serverSocket != -1) {
        close(serverSocket);
        serverSocket = -1;
    }

    std::cout << "Server stopped!" << std::endl;
}
