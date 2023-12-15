#include "header/server.hpp"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>



UDPServer::UDPServer(int port) {
    serverSocket = socket(AF_INET, SOCK_DGRAM, 0);

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(port);

    bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
}

UDPServer::~UDPServer() {
    close(serverSocket);
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

        int bytesRead = recvfrom(serverSocket, buffer.data(), buffer.size(), 0, (struct sockaddr*)&clientAddress, &clientSize);
        if (bytesRead > 0) {            
            std::lock_guard<std::mutex> lock(mutex);
            if(SERVER_RECV_DEBUG){
                for (char ch : buffer)
                    std::cout << int(ch) << " ";
                std::cout << std::endl;
                twt::Packet pack = twt::deserializePacket(buffer);
 
                std::cout << "Type:" << pack.type << std::endl;
                std::cout << "Time:" << pack.timestamp << std::endl;
                std::cout << "SeqN:" << pack.sequence_number << std::endl;
                std::cout << "PayL:";
                for (char ch : pack.payload)
                    std::cout << int(ch) << " ";
                std::cout << std::endl;
                
            }
            processingBuffer.push({clientAddress, buffer});

        }
    }
}

void UDPServer::processPacket() {
    while (running) {
        std::lock_guard<std::mutex> lock(mutex);
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
                    returnMessage = "Message request received\nSender ID: " + std::to_string(payload.first) + "\nMessage: " + payload.second + "\n";
                    sendto(serverSocket, returnMessage.c_str(), BUFFER_SIZE, 0, (struct sockaddr*)&clientAddress, sizeof(clientAddress));
                    break;
                }
                case twt::PacketType::Follow: {
                    std::pair<int, std::string> payload = twt::deserializeFollowPayload(packet);
                    int followerId = payload.first;
                    std::string usernameToFollow = payload.second;

            
                    int follewedId = usersList.getUserId(usernameToFollow);
                    if (follewedId == -1) {  // User not found
                        returnMessage = "User not found. Unable to follow.\n";

                    } else if (followerId == follewedId) { // User cannot follow himself
                        returnMessage = "You cannot follow yourself. Try following someone else.\n";

                    } else if (followers.isFollowing(followerId, follewedId)) { // User already following
                        returnMessage = "You are already following " + usernameToFollow + ".\n";

                    } else {
                        followers.follow(followerId, follewedId);
                        std::vector<twt::UserInfo> users_vector;
                        users_vector = usersList.storageMap();
                        write_file(database_name, users_vector);
                        returnMessage = "You are now following " + usernameToFollow + ".\n";
                    }
                    
                    sendto(serverSocket, returnMessage.c_str(), BUFFER_SIZE, 0, (struct sockaddr*)&clientAddress, sizeof(clientAddress));
                    break;
                }
                case twt::PacketType::Login: {
                    std::string username = twt::deserializeLoginPayload(packet);
                    loginBuffer.push({clientAddress, username});
                    returnMessage = "Login request received\nusername: " + username + "\n";
                    break;
                }
                case twt::PacketType::Exit: {
                    int accountId = twt::deserializeExitPayload(packet);
                    handleLogout(clientAddress, accountId);
                    returnMessage = "\n> Exit request received\nUserId: " + std::to_string(accountId) + "\n";
                    std::cout << returnMessage;
                    sendto(serverSocket, returnMessage.c_str(), BUFFER_SIZE, 0, (struct sockaddr*)&clientAddress, sizeof(clientAddress));

                    break;
                }
            }
        } else {
            sleep(0.1);
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
        if (!loginBuffer.empty()){
            std::lock_guard<std::mutex> lock(mutex);
            std::pair<const sockaddr_in&, const std::string&> pkt = loginBuffer.front();
            sockaddr_in clientAddress = pkt.first;
            std::string username = pkt.second;

            // std::cout << "username: " << username << std::endl;

            int id = usersList.createSession(username);
            
            if (id == -1){
                // Usuário já está logado, enviar uma resposta
                std::string replyMessage = "LOGIN_FAIL";
                sendto(serverSocket, replyMessage.c_str(), BUFFER_SIZE, 0, (struct sockaddr*)&clientAddress, sizeof(clientAddress));
            }
            else{
                // Enviar resposta de sucesso
                std::string replyMessage = "LOGIN_SUCCESS " + std::to_string(id);
                connectedUsers[id].push_back(clientAddress);
                //broadcastMessage(id);
                sendto(serverSocket, replyMessage.c_str(), BUFFER_SIZE, 0, (struct sockaddr*)&clientAddress, sizeof(clientAddress));
            }

            loginBuffer.pop();
        } else {
            sleep(0.1);
        }
    }
}

void UDPServer::processMessages(){
    while(running){
        std::lock_guard<std::mutex> lock(mutex);
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
            sleep(0.1);
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
    std::cout << "\033[1;36mFollowers List:\033[0m\n";
    for (auto user : this->followers.getFollowersList()) {
        if(!this->followers.getFollowers(user.first).empty()){
            std::cout << "User \033[1;33m" << usersList.getUsername(user.first) << "\033[0m followers: ";
            for (auto follower : user.second)
                std::cout << "\033[1;32m" << usersList.getUsername(follower) << " \033[0m";
            std::cout << std::endl;
        }
    }

    std::unordered_map<int, twt::UserInfo> allUsers = this->getUsersList();
    for (const auto& user : allUsers) {
        if (this->followers.getFollowers(user.first).empty()) {
            std::cout << "User \033[1;33m" << usersList.getUsername(user.first) << "\033[0m has no followers\n";
        }
    }

}

int main() {
    UDPServer udpServer(PORT);
    udpServer.start();

    return 0;
}
