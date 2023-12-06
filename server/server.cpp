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

    while (running) {
        // Processamento adicional pode ser feito aqui
    }

    processRequestsThread.join();
    processPacketsThread.join();
    processMessageThread.join();
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
                    //handleMessage(clientAddress, payload.first, payload.second);
                    returnMessage = "Message request received\nSender ID: " + std::to_string(payload.first) + "\nMessage: " + payload.second + "\n";
                    break;
                }
                case twt::PacketType::Follow: {
                    std::pair<int, std::string> payload = twt::deserializeFollowPayload(packet);
                    int followerId = payload.first;
                    std::string username = payload.second;
                    returnMessage = "Follow request received\nFollower ID: " + std::to_string(followerId) + "\nUsername: " + username + "\n";
                    break;
                }
                case twt::PacketType::Login: {
                    std::string username = twt::deserializeLoginPayload(packet);
                    //handleLogin(clientAddress, username);
                    returnMessage = "Login request received\nusername: " + username + "\n";
                    break;
                }
                case twt::PacketType::Exit: {
                    int accountId = twt::deserializeExitPayload(packet);
                    // Handle exit logic if needed
                    //handleLogout(clientAddress, accountId);
                    returnMessage = "Exit request received\nUserId: " + std::to_string(accountId) + "\n";
                    break;
                }
            }
            char bits[BUFFER_SIZE] = {0};
            for (int i = 0; i < (int)returnMessage.size(); i ++)
                bits[i] = returnMessage[i];
            std::cout << returnMessage << std::endl;

            sendto(serverSocket, bits, BUFFER_SIZE, 0, (struct sockaddr*)&clientAddress, sizeof(clientAddress));

        } else {
            sleep(0.1);
        }
    }
}

void UDPServer::handleLogout(const sockaddr_in& clientAddress, int id) {
    this->usersList.removeUser(id);

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


void UDPServer::handleLogin(const sockaddr_in& clientAddress, const std::string& username) {
    std::lock_guard<std::mutex> lock(mutex);
    int id = usersList.createSession(username);

    if (id == -1){
        // Usuário já está logado, enviar uma resposta
        std::string replyMessage = "LOGIN_FAIL";
        sendto(serverSocket, replyMessage.c_str(), replyMessage.length(), 0, (struct sockaddr*)&clientAddress, sizeof(clientAddress));
    }
    else{
        // Enviar resposta de sucesso
        std::string replyMessage = "LOGIN_SUCCESS " + std::to_string(id);
        connectedUsers[id].push_back(clientAddress);
        broadcastMessage(id);
        sendto(serverSocket, replyMessage.c_str(), replyMessage.length(), 0, (struct sockaddr*)&clientAddress, sizeof(clientAddress));
    }
}

void UDPServer::processMessages(){
    while(running){
        std::lock_guard<std::mutex> lock(mutex);
        if (!messageBuffer.empty()){
            twt::Message msg = messageBuffer.front();
            std::unordered_set<int> userFollowers = this->followers.getFollowers(msg.sender.userId);
            for (auto f : userFollowers){
                userMessageBuffer[f].push(msg);
                if (!connectedUsers[f].empty()){
                    broadcastMessage(f);
                }
            }
            messageBuffer.pop();
        }
    }
}

void UDPServer::broadcastMessage(int receiverId) {
    std::lock_guard<std::mutex> lock(mutex);
    while (!userMessageBuffer[receiverId].empty()){ 
        twt::Message message = userMessageBuffer[receiverId].front();
        for (const sockaddr_in& userAddr : connectedUsers[receiverId]){
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
