#ifndef CONNECTIONS_H
#define CONNECTIONS_H
#include "../Header.h"

#define SLOTS_COUNT 15

namespace Connections
{
    struct Connection
    {
        sockaddr_in address;
        socklen_t length;
        
        int socket;

        std::thread eventListener;
        std::thread::id eventListnerThreadId;

        bool alive;
        std::thread verifications;
    };
    void Detect(int _socketId);
    void SendData(int _socketId, const char* _contentBuffer);
    void Remove(int _socketId);
    void Kick(int _socketId, const char* _reason);
    int GetOwnerServerId(void);
    void Stop(void);
}
#endif // CONNECTIONS_H