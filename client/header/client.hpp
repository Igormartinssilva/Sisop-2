// client.hpp

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../../asserts/constraints.hpp"
#include <iostream>
#include <cstring>
#include <vector>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include "../../common/header/data.hpp"

class Client {
public:
    Client();
    ~Client();

    void setServer(const char *hostname);
    
    // New functionalities
    void sendLogin(const std::string& username);
    void sendFollow(int followerId, const std::string& username);
    void sendMessage(int senderId, const std::string& message);
    void sendExit(int accountId);

private:
    int sockfd;
    struct sockaddr_in serv_addr;

    // Refactored function
    void sendPackage(twt::MessageType type, const std::vector<char>& payload);
};

#endif // CLIENT_HPP
