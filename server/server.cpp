#include "header/server.hpp"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>



UDPServer::UDPServer(int port) {
    serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverSocket < 0) {
        perror("Error creating socket");
        return;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(port);

    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("Error binding socket to port");
        return;
    }
}

UDPServer::~UDPServer() {
}

constexpr char RED[] = "\033[1;31m";
constexpr char GREEN[] = "\033[1;32m";
constexpr char YELLOW[] = "\033[1;33m";
constexpr char BLUE[] = "\033[1;34m";
constexpr char RESET[] = "\033[0m";
void printMenu() {
    std::cout << BLUE << ">>-- Welcome to Y --<<" << RESET << std::endl << std::endl; 
    std::cout << RED << "1. " << RESET << "Display User List\n";
    std::cout << RED << "2. " << RESET << "Display Followers List\n";
    std::cout << RED << "3. " << RESET << "Exit\n";
    std::cout << BLUE << "Choose an option: " << RESET;
}

void clearScreen() {
#ifdef _WIN32
    std::system("cls");
#else
    std::system("clear");
#endif
}

void pressEnterToContinue() {
    std::cout << YELLOW << "\n[Press Enter to Continue]" << RESET;
    std::cin.ignore(); // Wait for Enter key press
}


void UDPServer::start() {
    std::cout << "Server listening on port " << PORT << "...\n";
    loadDataBase();
    running = true;
    std::thread processRequestsThread(&UDPServer::handlePackets, this);
    std::thread processPacketsThread(&UDPServer::processPacket, this);
    std::thread processMessageThread(&UDPServer::processMessages, this);
    std::thread processLoginThread(&UDPServer::processLogin, this);

    while (running) {
        int choice;
        clearScreen();
        printMenu();
        std::cin >> choice;
        std::cin.ignore(); // Consume newline character

        switch (choice) {
            case 1: {
                displayUserList();
                pressEnterToContinue();
                break;
            }
            case 2: {
                displayFollowersList();
                pressEnterToContinue();
                break;
            }
            case 3: {
                std::cout << "Exiting the application.\n";
                pressEnterToContinue();
                running = false;
                break;
            }
            default:
                std::cout << "Invalid choice. Please try again.\n";
                std::cin.ignore();
                pressEnterToContinue();
        }
    }

    processRequestsThread.join();
    processPacketsThread.join();
    processMessageThread.join();
    processLoginThread.join();
}

void UDPServer::handlePackets() {
    while (running) {
        sockaddr_in clientAddress;
        socklen_t clientSize = sizeof(clientAddress);

        std::vector<char> buffer(BUFFER_SIZE);
        memset(buffer.data(), 0, sizeof(buffer));

        std::cout << "Waiting for recvfrom" << std::endl;
        int bytesRead = recvfrom(serverSocket, buffer.data(), buffer.size(), 0, (struct sockaddr*)&clientAddress, &clientSize);
        if (bytesRead > 0) {            
            std::lock_guard<std::mutex> lock(mutexProcBuff);
            processingBuffer.push({clientAddress, buffer});

        }
    }
}

void UDPServer::processPacket() {
    while (running) {
        std::lock_guard<std::mutex> lock(mutexProcBuff);
        if (!processingBuffer.empty()) {
            std::string returnMessage("unknown type");
            std::pair<const sockaddr_in&, const std::vector<char>&> bufferValue = processingBuffer.front();
            const sockaddr_in& clientAddress = bufferValue.first;
            std::vector<char> packet = bufferValue.second;
            processingBuffer.pop();

            twt::Packet pack = twt::deserializePacket(packet);

            switch (pack.type) {
                case twt::PacketType::Mensagem: {
                    std::pair<int, std::string> payload = twt::deserializeMessagePayload(packet);
                    messageBuffer.push({{usersList.getUsername(payload.first), payload.first}, payload.second});
                    returnMessage = "ACK_MSG,Message request received\nSender ID: " + std::to_string(payload.first) + "\nMessage: " + payload.second + "\n";
                    sendto(serverSocket, returnMessage.c_str(), BUFFER_SIZE, 0, (struct sockaddr*)&clientAddress, sizeof(clientAddress));
                    break;
                }
                case twt::PacketType::Follow: {
                    std::cout << "Follow request received" << std::endl;
                    std::pair<int, std::string> payload = twt::deserializeFollowPayload(packet);
                    int followerId = payload.first;
                    std::string usernameToFollow = payload.second;

            
                    int follewedId = usersList.getUserId(usernameToFollow);
                    if (follewedId == -1) {  // User not found
                        returnMessage = "ACK_FLW,User not found. Unable to follow.\n";

                    } else if (followerId == follewedId) { // User cannot follow himself
                        returnMessage = "ACK_FLW,You cannot follow yourself. Try following someone else.\n";

                    } else if (followers.isFollowing(followerId, follewedId)) { // User already following
                        returnMessage = std::string("ACK_FLW,You are already following ") + usernameToFollow + std::string(".\n");

                    } else {
                        followers.follow(followerId, follewedId);
                        saveDataBase();
                        returnMessage = std::string("ACK_FLW,You are now following ") + usernameToFollow +  std::string(".\n");
                    }

                    std::cout << returnMessage << std::endl;
                    sendto(serverSocket, returnMessage.c_str(), BUFFER_SIZE, 0, (struct sockaddr*)&clientAddress, sizeof(clientAddress));
                    break;
                }
                case twt::PacketType::Login: {
                    std::string username = twt::deserializeLoginPayload(packet);
                    loginBuffer.push({clientAddress, username});
                    break;
                }
                case twt::PacketType::Exit: {
                    int accountId = twt::deserializeExitPayload(packet);
                    handleLogout(clientAddress, accountId);
                    returnMessage = "ACK_EXT,Exit request received\nUserId: " + std::to_string(accountId) + "\n";
                    std::cout << returnMessage;
                    sendto(serverSocket, returnMessage.c_str(), BUFFER_SIZE, 0, (struct sockaddr*)&clientAddress, sizeof(clientAddress));

                    break;
                }
            }
        } else {
            
        }
    }
}

void UDPServer::handleLogout(const sockaddr_in& clientAddress, int id) {
    this->usersList.logout(id);

    // Find the connected sessions for the user
    std::vector<sockaddr_in>& sessions = this->connectedUsers[id];

    // Iterate through the connected sessions and find the one to be removed
    auto pos = std::find_if(sessions.begin(), sessions.end(), [&](const sockaddr_in& session) {
        // Compare relevant fields (IP address and port) individually
        return session.sin_addr.s_addr == clientAddress.sin_addr.s_addr &&
               session.sin_port == clientAddress.sin_port;
    });

    // Check if the session was found before erasing
    if (pos != sessions.end()) {
        sessions.erase(pos);
    }
}


void UDPServer::processLogin() {
    while(running) {
        std::lock_guard<std::mutex> lock(mutexLogBuff);
        if (!loginBuffer.empty()){
            std::pair<const sockaddr_in&, const std::string&> pkt = loginBuffer.front();
            sockaddr_in clientAddress = pkt.first;
            std::string username = pkt.second;

            // std::cout << "username: " << username << std::endl;

            int id = usersList.createSession(username);
            std::string replyMessage = std::string("ACK_LOG,") + std::to_string(id) + std::string(",") + username.c_str() + std::string(",");
            
            if (id != -1) {
                saveDataBase();
                connectedUsers[id].push_back(clientAddress);
                //broadcastMessage(id);
            }
            std::cout << "sending ack for login: " << username << " id: " << id << std::endl;
            sendto(serverSocket, replyMessage.c_str(), BUFFER_SIZE, 0, (struct sockaddr*)&clientAddress, sizeof(clientAddress));
            // Obter o endereço IP do cliente como uma string
            std::string clientIPAddress = inet_ntoa(clientAddress.sin_addr);

            // Imprimir o endereço IP do cliente na tela
            std::cout << "Endereço IP do cliente: " << clientIPAddress << std::endl;
            loginBuffer.pop();
        } else {
            
        }
    }
}
 
void UDPServer::processMessages(){
    while(running){
        std::lock_guard<std::mutex> lock(mutexMsgBuff);
        if (!messageBuffer.empty()){
            twt::Message msg = messageBuffer.front();
            std::cout << msg.sender.userId << std::endl;
            std::unordered_set<int> userFollowers = this->followers.getFollowers(msg.sender.userId);
            std::cout << "Lista de followers de " << msg.sender.userId << " - " << msg.sender.username << ": ";
            for (auto i : userFollowers) std::cout << i << ", ";
            std::cout << std::endl;
            for (auto f : userFollowers){
                userMessageBuffer[f].push(msg);
                if (!connectedUsers[f].empty()){
                    broadcastMessage(f);
                }
            }
            messageBuffer.pop();
        } else {
            
        }
    }
}

void UDPServer::broadcastMessage(int receiverId) {
    while (!userMessageBuffer[receiverId].empty()){ 
        twt::Message message = userMessageBuffer[receiverId].front();
        for (const sockaddr_in& userAddr : connectedUsers[receiverId]){
            std::cout << "\n> Sending message: \"" << message.content.c_str() << 
                "\" to user @"<< usersList.getUsername(receiverId) << "(id " << std::to_string(receiverId) << ")" <<
                " from user @" << message.sender.username << " (id " << message.sender.userId << ")" << std::endl;
            std::string str(
                message.sender.username + ',' + 
                std::to_string(message.sender.userId) + ',' +
                message.content
            );
            sendto(serverSocket, str.c_str(), str.length(), 0, (struct sockaddr*)&userAddr, sizeof(userAddr));
        }
        userMessageBuffer[receiverId].pop();
    }
}


std::unordered_map<int, twt::UserInfo> UDPServer::getUsersList() {
    return this->usersList.getUserListInfo();
}

void UDPServer::displayUserList() {
    std::cout << "User List:\n";
    for (auto user : this->getUsersList())
        user.second.display();
    }

void UDPServer::displayFollowersList() {
    std::unordered_map<int, twt::UserInfo> allUsers = this->getUsersList();
    for (auto user : allUsers) {
        std::unordered_set<int> followList = followers.getFollowers(user.first);
        if (followList.empty()) {
            std::cout << "User \033[1;33m" << usersList.getUsername(user.first) << "\033[0m has no followers\n";
        } else {
            std::cout << "User \033[1;33m" << usersList.getUsername(user.first) << "\033[0m followers: ";
            for (int follower : followList)
                std::cout << "\033[1;32m" << usersList.getUsername(follower) << " (" << std::to_string(follower) << ") ";
            std::cout << "\033[0m" << std::endl;
        }
    }

}

void UDPServer::saveDataBase(){
    std::vector<twt::UserInfo> users_vector;
    loadFollowersIntoUsersList();
    users_vector = usersList.storageMap();
    // Para cada userId na usersList
    for (int userId : usersList.getUserIds()) {
        // Obter o nome de usuário para o userId
        std::string username = usersList.getUsername(userId);

        // Imprimir o nome de usuário
        std::cout << "User ID: " << userId << ", Username: " << username << std::endl;

        // Obter os seguidores do usuário
        std::unordered_set<int> followers = usersList.getUser(userId).getFollowers();

        // Imprimir os seguidores do usuário
        std::cout << "Followers: ";
        for (int followerId : followers) {
            std::cout << followerId << " ";
        }
        std::cout << std::endl;
    }
    write_file(database_name, users_vector);
}

void UDPServer::loadDataBase(){
    std::vector<twt::UserInfo> users_vector;
    users_vector = read_file(database_name);
    usersList.loadMap(users_vector);
    saveFollowersFromUsersList();
    usersList.setNextId(findMaxUserId(users_vector)+1);
}


int main() {
    UDPServer udpServer(PORT);
    udpServer.start();

    return 0;
}

void UDPServer::loadFollowersIntoUsersList() {
    // Para cada userId na usersList
    for (int userId : usersList.getUserIds()) {
        // Obter os seguidores do userId
        std::unordered_set<int> followerIds = followers.getFollowers(userId);

        // Para cada seguidor, adicione-o à lista de seguidores do usuário correspondente na usersList
        for (int followerId : followerIds) {
            twt::UserInfo& userInfo = usersList.getUser(followerId);
            userInfo.getFollowers().insert(userId);
        }
    }
}

void UDPServer::saveFollowersFromUsersList() {
    // Para cada userId na usersList
    for (int userId : usersList.getUserIds()) {
        // Obter o UserInfo para o userId
        twt::UserInfo& userInfo = usersList.getUser(userId);

        // Para cada seguidor do usuário, adicione-o à lista de seguidores
        for (int followerId : userInfo.getFollowers()) {
            followers.follow(followerId, userId);
        }
    }
}