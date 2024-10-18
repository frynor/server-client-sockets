#include "network.h"
#include <cerrno>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

bool Client::connect() {
	if (!createSocket()) {
		return false;
	}

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(SERVER_PORT);
	if (inet_pton(AF_INET, SERVER_IP, &serverAddress.sin_addr) <= 0) {
		std::cerr << "inet_pton() error!" << strerror(errno) << std::endl;
		return false;
	}

	if (::connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1) {
		std::cerr << "Can not connect client to the server!" << strerror(errno) << std::endl;
		return false;
	}
	return true;

}

bool Client::sendMessage(const std::string& message) {
	if (send(clientSocket, message.c_str(), message.length(), 0) == -1) {
		std::cerr << "Cannot send message to the server!" << strerror(errno) << std::endl;
		return false;
	}
	return true;
}

std::string Client::receiveMessage() {
	char buffer[1024] = {0};
	int byteReceived = recv(clientSocket, buffer, 1024, 0);
	if (byteReceived == -1) {
		std::cerr << "Can not receive the message!" << strerror(errno) << std::endl;
		return "";
	}
	return std::string(buffer, byteReceived);
}

void Client::disconnect() {
	close(clientSocket);
}

Client::Client() : clientSocket(-1) {}

Client::~Client() {
	if (clientSocket == -1) {
		disconnect();
	}
}
