#pragma once

#include "client.hpp"
#include "../../common/header/data.hpp"
#include "message.hpp"
#include <fstream>

class Session {
private:
    bool running;

    Client client;
    twt::UserInfo user;
    std::queue<std::pair<const sockaddr_in&, const std::string&>> receivingBuffer;
    int clientSocket;

public:
    Session();
    void processReceiving();
    void processBuffer();
    void sendLogin(const std::string& username);
    void sendFollow(const std::string& username);
    void sendMessage(const std::string& messageContent);
    void sendExit();
    
};
