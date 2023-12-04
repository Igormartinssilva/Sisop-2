#pragma once

#include "client.hpp"
#include "user.hpp"
#include "../../common/header/data.hpp"
#include "message.hpp"
#include <fstream>

class Session {
private:
    Client client;
    twt::UserInfo user;

public:
    Session();
    void sendLogin(const std::string& username);
    void sendFollow(const std::string& username);
    void sendMessage(const std::string& messageContent);
    void sendExit();
};
