#pragma once

#include "client.hpp"
#include "user.hpp"
#include "../../common/header/data.hpp"
#include "message.hpp"
#include <fstream>

class Session {
private:
    Client client;

public:
    Session();
    void sendMessage(Message msg);
};
