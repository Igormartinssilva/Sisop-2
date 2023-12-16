#pragma once

#include "client.hpp"
#include "../../common/header/data.hpp"
#include "../../server/header/server.hpp"
#include <fstream>
#include <thread>
#include <mutex>

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
    void waitForAck();
    twt::Message decodeMessage(std::string msg);
    

public:
    Session();
    Session(std::string);

    void sendLogin(const std::string& username);
    void sendFollow(const std::string& username);
    void sendMessage(const std::string& message);
    void sendExit();
    void printYourMessages();
    
    bool isLogged();
};


