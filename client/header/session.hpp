#pragma once

#include "client.hpp"
#include "../../common/header/data.hpp"
#include "message.hpp"
#include "../../server/header/server.hpp"
#include <fstream>

class Session {
private:
    bool running;
    bool logged;

    Client client;
    twt::User user;
    std::queue<std::pair<const sockaddr_in&, const std::string&>> receivingBuffer;
    int clientSocket;

public:
    Session();
    Session(std::string);
    bool isLogged();
    void processReceiving();
    void processBuffer();
    void sendLogin(const std::string& username);
    void sendFollow(const std::string& username);
    void sendMessage(const std::string& messageContent);
    void sendExit();
    
};
