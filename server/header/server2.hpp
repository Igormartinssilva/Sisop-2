#ifndef SERVER
#define SERVER
#include <iostream>
#include <vector>
#include <unistd.h>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <cstring>
#include <arpa/inet.h>
#include "../../common/header/data.hpp"
#include "../../asserts/constraints.hpp"

class UDPServer {
public:
    UDPServer(int port);
    ~UDPServer();
    void start();

private:
    void handlePackets();
    void processPacket();
    void processMessage();
    void handleLogin(const sockaddr_in& clientAddress, const std::string& username);
    void handleReadRequest(const sockaddr_in& clientAddress, const std::string& username);
    void broadcastMessage(const twt::Message& message);

    int serverSocket;
    std::queue<std::pair<const sockaddr_in&, const std::vector<char>>> processingBuffer;
    twt::Followers followers;
    twt::UsersList usersList;
    std::queue<twt::Message> messageBuffer;
    std::mutex mutex;
    std::condition_variable cv;
    bool running;
};

#endif