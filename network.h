#ifndef HOST_H
#define HOST_H

#include <string>
#include <stdint.h>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>

const char SERVER_IP[] = "127.0.0.1";
const int SERVER_PORT = 8080;

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
		bool createSocket();
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
		bool createSocket();
};

#endif  // HOST_H
