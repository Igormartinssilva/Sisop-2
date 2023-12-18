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
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <cstring>
#include <arpa/inet.h>
#include "../../common/header/data.hpp"
#include "../../assets/constraints.hpp"
#include "../../database/database.hpp"
#include "../../common/header/serialize.hpp"

class UDPServer {
public:
    UDPServer(int port);
    ~UDPServer();
    void start();
    twt::Followers followers;
    twt::UsersList usersList;
    std::string database_name = "assets/database.txt";

    void displayUserList();
    void displayFollowersList();

    std::unordered_map<int, twt::UserInfo> getUsersList();

    void resetSequenceNumber(const sockaddr_in& clientAddress);
    // Adicionando função para verificar se o pacote é repetido
    bool isPacketRepeated(const sockaddr_in& clientAddress, const twt::Packet& pack);
    void updateSequenceNumber(const sockaddr_in& clientAddress, uint16_t newSequenceNumber);
    bool isSequenceNumberValid(const sockaddr_in& clientAddress, const twt::Packet& pack);

private:

std::unordered_map<uint32_t, std::unordered_map<uint16_t, uint16_t>> lastSequenceNumber;

    

    void handlePackets();
    void loadFollowersIntoUsersList();
    void saveFollowersFromUsersList();
    void processPacket();
    void saveDataBase();
    void loadDataBase();
    void processMessages();
    void processLogin();
    void processPing();
    void processPingErase();
    void handleLogout(const sockaddr_in& clientAddress, int id);
    void sendBufferedMessages(int userId);
    
    void broadcastMessage(int receiverId);
    bool UserConnected(int userId);

    int serverSocket;
    std::queue<std::pair<const sockaddr_in&, const std::string>> processingBuffer;

    std::unordered_map<int, std::vector<sockaddr_in>> connectedUsers;  // User ID -> Set of connected sessions
    
    std::unordered_map<int, std::queue<twt::Message>> userMessageBuffer, msgToSendBuffer;  // User ID -> Queue of stored messages
    std::queue<twt::Message> messageBuffer; // Messages of the tr
    std::queue<std::pair<std::pair<int, std::pair<in_addr_t, in_port_t>>, sockaddr_in>> pingQueue;
    std::queue<std::pair<const sockaddr_in&, const std::string&>> loginBuffer;
    std::map<std::pair<int, std::pair<in_addr_t, in_port_t>>, sockaddr_in> pingSet;
    std::mutex mutexProcBuff;
    std::mutex mutexUsers;
    std::mutex mutexMsgBuff;
    std::mutex mutexLogBuff;
    std::mutex mutexLogPing;
    bool running;

    

};



#endif