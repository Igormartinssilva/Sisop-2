#pragma once

#include "client.hpp"
#include "../../common/header/data.hpp"
#include "../../server/header/server.hpp"
#include <fstream>
#include <thread>
#include <mutex>
#include <signal.h>
#include <chrono>

class Session {
private:
    std::thread processingThread;
    bool ackReceived;
    bool running;
    bool logged;
    Client client;
    twt::User user;
    void processBuffer();
    std::queue<std::string> messageBuffer;
    std::mutex bufferMutex;
    void processReceiving();
    std::string getMessageBuffer();
    bool packetTransmited();
    twt::Message decodeMessage(std::string msg);
    void transmitPacket(twt::PacketType type, const std::vector<char> payload);
    

public:
    Session();
    Session(std::string);
    ~Session();

    int sendLogin(const std::string& username);
    void sendFollow(const std::string& username);
    void sendMessage(const std::string& message);
    void sendExit();
    void printYourMessages();
    bool isLogged();
};


