#pragma once

#include "client.hpp"
#include "user.hpp"
#include "message.hpp"
#include <fstream>

class Session {
private:
    Client client;

public:
    Session();
    void sendMessage(Message msg);
};
