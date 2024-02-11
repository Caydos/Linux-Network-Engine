#include "Connections.h"
#include "../Events/Events.h"
#include "../Manager/Peds/Peds.h"

static std::shared_mutex mutex;
static int mainSocket;
static std::vector<Connections::Connection*> connections;

void Connections::Detect(int _socketId)
{
	mainSocket = _socketId;
    std::cout << "Detector started" << std::endl;
	while (true)
	{
        while (connections.size() > SLOTS_COUNT)
        {
           std::this_thread::sleep_for(std::chrono::seconds(5));
        }
        Connections::Connection* connection = new Connections::Connection;
        connection->length = sizeof(connection->address);
		
		listen(_socketId, 5);
		connection->socket = accept(_socketId, (sockaddr*)&connection->address, &connection->length);
		if (connection->socket < 0)
		{
			std::cout << "Failed to accept Connection" << std::endl;
			delete connection;
		}
		else
		{
			connection->eventListener = std::thread(Events::Listener, connection->socket);
			connection->eventListnerThreadId = connection->eventListener.get_id();
			connection->eventListener.detach();
			connections.push_back(connection);
			Events::TriggerClient(connection->socket, "Connections::Established", "Welcome on the server !", connection->socket);
			std::cout << "New connection was created " << connection->socket << std::endl;
		}
	}
}

void Connections::SendData(int _socketId, const char* _contentBuffer)
{
	std::shared_lock<std::shared_mutex> lock(mutex);
	switch (_socketId)
	{
	case -2:
	{/* TO ALL */
		for (int i = 0; i < connections.size(); i++)
		{
			write(connections[i]->socket, _contentBuffer, strlen(_contentBuffer) + 1);
		}
		break;
	}
	case -1:
	{/* TO ALL EXCEPT ME */
		std::thread::id currenteventListnerThreadId = std::this_thread::get_id();
		for (int i = 0; i < connections.size(); i++)
		{
			if (connections[i]->eventListnerThreadId != currenteventListnerThreadId)
			{
				write(connections[i]->socket, _contentBuffer, strlen(_contentBuffer) + 1);
			}
		}
		break;
	}
	default:
	{/* TO ID */
		for (int i = 0; i < connections.size(); i++)
		{
			if (connections[i]->socket == _socketId)
			{
				write(connections[i]->socket, _contentBuffer, strlen(_contentBuffer) + 1);
				return;
			}
		}
		std::cout << "Connection with ID : " << _socketId << " not found." << std::endl;
		break;
	}
	}
}

void Connections::Remove(int _socketId)
{
	std::unique_lock<std::shared_mutex> lock(mutex);
	for (int i = 0; i < connections.size(); i++)
	{
		if (connections[i]->socket == _socketId)
		{
			close(connections[i]->socket);
			Peds::OwnerRemoval(connections[i]->socket);
			delete connections[i];
			connections.erase(connections.begin() + i);
			return;
		}
	}
}

void Connections::Kick(int _socketId, const char* _reason)
{
	// std::unique_lock<std::shared_mutex> lock(mutex);
	for (int i = 0; i < connections.size(); i++)
	{
		if (connections[i]->socket == _socketId)
		{
			Events::TriggerClient(connections[i]->socket, "Connections::Kicked", _reason);
			close(connections[i]->socket);
			Peds::OwnerRemoval(connections[i]->socket);
			delete connections[i];
			connections.erase(connections.begin() + i);
			return;
		}
	}
}

int Connections::GetOwnerServerId(void)
{
	std::thread::id currenteventListnerThreadId = std::this_thread::get_id();
	for (int i = 0; i < connections.size(); i++)
	{
		if (connections[i]->eventListnerThreadId == currenteventListnerThreadId)
		{
			return connections[i]->socket;
		}
	}
	return 0;
}

void Connections::Stop(void)
{
	for (size_t i = 0; i < connections.size(); i++)
	{
		Connections::Kick(connections[i]->socket, "Server Closing");
	}
	// std::unique_lock<std::shared_mutex> lock(mutex);
	close(mainSocket);
	exit(0);
}