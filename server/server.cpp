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

void UDPServer::start() {
    std::cout << "Server listening on port " << PORT << "...\n";

    running = true;
    std::thread processRequestsThread(&UDPServer::handlePackets, this);
    std::thread processPacketsThread(&UDPServer::processPacket, this);
    std::thread processMessageThread(&UDPServer::processMessages, this);
    std::thread processLoginThread(&UDPServer::processLogin, this);

    while (running) {
        // Processamento adicional pode ser feito aqui
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
                    messageBuffer.push({{"", payload.first}, payload.second});
                    returnMessage = "Message request received\nSender ID: " + std::to_string(payload.first) + "\nMessage: " + payload.second + "\n";
                    sendto(serverSocket, returnMessage.c_str(), BUFFER_SIZE, 0, (struct sockaddr*)&clientAddress, sizeof(clientAddress));
                    break;
                }
                case twt::PacketType::Follow: {
                    std::pair<int, std::string> payload = twt::deserializeFollowPayload(packet);
                    int followerId = payload.first;
                    std::string usernameToFollow = payload.second;

                    //codigo do igor para follow                 
                    // Check if the username exists and get its ID
                    int followeeId = usersList.getUserId(usernameToFollow);
                    if (followeeId != -1) {
                        // Check if the follower is not already following the user
                        if (!followers.isFollowing(followerId, followeeId)) {
                            // Perform the follow operation
                            followers.follow(followerId, followeeId);
                            write_file("database.txt", usersList.mapToVector());

                            // Return a success message
                            returnMessage = "Follow request received 1\nFollower ID: " + std::to_string(followerId) +
                                            "\nUsername: " + usernameToFollow + "\nFollow successful\n";
                        } else {
                            // Return a message indicating that the follower is already following the user
                            returnMessage = "Follow request received 2\nFollower ID: " + std::to_string(followerId) +
                                            "\nUsername: " + usernameToFollow + "\nAlready following\n";
                        }
                    } else {
                        // Return a message indicating that the username does not exist
                        returnMessage = "Follow request received 3\nFollower ID: " + std::to_string(followerId) +
                                        "\nUsername: " + usernameToFollow + "\nUser not found\n";
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
                    returnMessage = "Exit request received\nUserId: " + std::to_string(accountId) + "\n";
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

            std::cout << "username: " << username << std::endl;

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
            std::cout << "Lista de followers de " << msg.sender.userId << "-" << msg.sender.username << ": ";
            for (auto i : userFollowers) std::cout << i << ", ";
            std::cout << std::endl;
            for (auto f : userFollowers){
                std::cout << "entrou na lista de followers" << msg.content << std::endl;
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
    std::lock_guard<std::mutex> lock(mutex);
    std::cout << "Chegou (diferente do santos)" << receiverId << std::endl;
    while (!userMessageBuffer[receiverId].empty()){ 
        twt::Message message = userMessageBuffer[receiverId].front();
        for (const sockaddr_in& userAddr : connectedUsers[receiverId]){
            std::cout << "Sending message: " << message.content.c_str() << " to user " << std::to_string(receiverId) << "from user " << message.sender.username << " (id " << message.sender.userId << ")" << std::endl;
            sendto(serverSocket, message.content.c_str(), message.content.length(), 0, (struct sockaddr*)&userAddr, sizeof(userAddr));
        }
        userMessageBuffer[receiverId].pop();
    }
}

int main() {
    UDPServer udpServer(PORT);
    udpServer.start();

    return 0;
}
