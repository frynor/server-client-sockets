#include "network.h"

int main() {
	Server server;
	if (server.start()) {
		server.run();
	}
	return 0;
}
