#pragma once
#ifndef UDP_CLIENT_H
#define UDP_CLIENT_H
#include "../../asserts/constraints.hpp"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "message.hpp"

class Client {
private:
    int sockfd;
    struct sockaddr_in serv_addr;

public:
    Client();
    ~Client();
    void setServer(const char *hostname);
    void sendMessage(struct Bitstream);
};

#endif // UDP_CLIENT_H
