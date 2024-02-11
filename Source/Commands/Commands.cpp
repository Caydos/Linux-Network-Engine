#include "Commands.h"
#include "../Header.h"
#include "../Connections/Connections.h"

void Commands::Thread()
{
    std::string command;
    while (true)
    {
        std::getline(std::cin, command);
        if (command == "close")
        {
            Connections::Stop();
        }
        command.clear();
        break;
    }
    exit(0);
}
