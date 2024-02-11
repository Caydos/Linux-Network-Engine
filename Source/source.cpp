#include "Header.h"
#include "Connections/Connections.h"
#include "Scripting/Sync/Sync.h"
#include "Commands/Commands.h"

#define SERVER_PORT 55301
int socketId;

int main(void)
{
	std::thread commandThread(&Commands::Thread);
	std::cout << "Server started" << std::endl;
	// ListEvents();
	sockaddr_in serv_addr;

	socketId = socket(AF_INET, SOCK_STREAM, 0);
	if (socketId < 0)
	{
		std::cout << "ERROR opening socket" << std::endl;
		exit(EXIT_FAILURE);
	}

	bzero((char*)&serv_addr, sizeof(serv_addr));
	int port = SERVER_PORT;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(port);
	int bd = bind(socketId, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    
	if (bd < 0)
	{
		std::cerr << "Bind failed: " << strerror(errno) << std::endl;
		close(socketId);
		exit(EXIT_FAILURE);
	}
	Sync::Events();
	Connections::Detect(socketId);

	close(socketId);
	std::cout << "Server Closing" << std::endl;
	return 0;

}