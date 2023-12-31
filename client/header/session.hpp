#pragma once

#include "client.hpp"
#include "../../common/header/data.hpp"
#include "message.hpp"
#include <fstream>

class Session {
private:
    bool running;
    bool logged;

    Client client;
    twt::UserInfo user;
    std::queue<std::pair<const sockaddr_in&, const std::string&>> receivingBuffer;
    std::queue<twt::Message> notificationBuffer;
    int clientSocket;

public:
    Session();
    Session(std::string);
    bool isLogged();
    void processReceiving();
    void processBuffer();
    void processNotifBuffer();
    void sendLogin(const std::string& username);
    void sendFollow(const std::string& username);
    void sendMessage(const std::string& messageContent);
    void sendExit();
    
};
