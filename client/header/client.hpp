// client.hpp

#ifndef CLIENT_HPP
#define CLIENT_HPP
#define DEBUG_CLIENT false

#include "../../common/header/data.hpp"
#include "../../common/header/serialize.hpp"
#include "../../assets/constraints.hpp"
#include <condition_variable>
#include <thread>
#include <iostream>
#include <cstring>
#include <vector>
#include <arpa/inet.h>
#include <unistd.h>
#include <mutex>
#include <netdb.h>

class Client {
public:
    Client();
    ~Client();
    void setServer(const char *hostname);

    std::queue<std::string> receivingBuffer;
    std::mutex bufferMutex;
    std::condition_variable bufferCondVar;
    void decodeReceiving();
    std::string getBuffer();
    int sendPacket(twt::PacketType type, uint16_t timeStamp, const std::string& payload);
    
private:
    uint16_t sequence_number;
    std::thread receivingThread;
    struct sockaddr_in serv_addr;
    int clientSocket;
    int sockfd;
    void processReceiving();
};

#endif // CLIENT_HPP
