#ifndef HOST_H
#define HOST_H

#include <cstring>
#include <string>
#include <stdint.h>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>

const char SERVER_IP[] = "127.0.0.1";
const int SERVER_PORT = 8080;

class SocketCreator {
	public:
		static int createSocket(const char* errorMessage) {
			int sock = socket(AF_INET, SOCK_STREAM, 0);
			if (sock == -1) {
				std::cerr << errorMessage << strerror(errno) << std::endl;
		}
		return sock;
	}
};

class Server {
	private:
		int serverSocket;
		std::vector<int> clientSockets;
	public:
		Server();
		~Server();
		bool start();
		void run();
		void stop();
	private:
		bool createSocket() {
			serverSocket = SocketCreator::createSocket("Can not create a server socket: ");
			return serverSocket != -1;
	}
		bool bindSocket();
		bool listenForConnections(int backlog = 5);
		int acceptClient();
		void handleClient(int clientSocket);
		void closeClientSocket(int clientSocket);
};

class Client {
	private:
		int clientSocket;
		struct sockaddr_in serverAddress;
	public:
		Client();
		~Client();
		bool connect();
		bool sendMessage(const std::string& message);
		std::string receiveMessage();
		void disconnect();
	private:
		bool createSocket() {
			clientSocket = SocketCreator::createSocket("Can not create a client socket: ");
			return clientSocket != -1;
	}
};

#endif  // HOST_H
