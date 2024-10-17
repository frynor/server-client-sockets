#include "network.h"
#include <iostream>

int main() {
	Client client;

	if (client.connect()) {
		std::cout << "Connected to the server!" << std::endl;

		std::string message;
		while (true) {
			std::cout << "Enter a message (or 'quit' to disconnect): ";
			std::getline(std::cin, message);

			if (message == "quit") {
				break;
			}

			if (client.sendMessage(message)) {
				std::cout << "Message sent: " << message << std::endl;

				std::string response = client.receiveMessage();
				if (!response.empty()) {
					std::cout << "Received: " << response << std::endl;
				} else {
					std::cout << "No response from the server." << std::endl;
				}
			} else {
				std::cerr << "Failed to send the message!" << std::endl;
				break;
			}
		}
		client.disconnect();
		std::cout << "Disconnected from the server!" << std::endl;
	} else {
		std::cerr << "Failed to connect to the server!" << std::endl;
	}

	return 0;
}
