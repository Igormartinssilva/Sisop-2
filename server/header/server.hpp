#ifndef SERVER
#define SERVER
#define SERVER_RECV_DEBUG false

#include <iostream>
#include <vector>
#include <set>
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
#include "../../database/database.hpp"

class UDPServer {
public:
    UDPServer(int port);
    ~UDPServer();
    void start();
    twt::Followers followers;
    twt::UsersList usersList;
    std::string database_name = "database.txt";

    void displayUserList();
    void displayFollowersList();

    std::unordered_map<int, twt::UserInfo> getUsersList();
private:
    void handlePackets();
    void processPacket();
    void saveDataBase();
    void loadDataBase();
    void processMessages();
    void processLogin();
        
    void handleLogout(const sockaddr_in& clientAddress, int id);
    
    
    void broadcastMessage(int receiverId);


    int serverSocket;
    std::queue<std::pair<const sockaddr_in&, const std::vector<char>>> processingBuffer;

    std::unordered_map<int, std::vector<sockaddr_in>> connectedUsers;  // User ID -> Set of connected sessions
    
    std::unordered_map<int, std::queue<twt::Message>> userMessageBuffer;  // User ID -> Queue of stored messages
    std::queue<twt::Message> messageBuffer; // Messages of the tr
    std::queue<std::pair<const sockaddr_in&, const std::string&>> loginBuffer;
    

    
    
    std::mutex mutex;
    bool running;
};

#endif