#include "header/server2.hpp"

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
    std::thread processPacketThread(&UDPServer::handlePackets, this);
    std::thread processMessageThread(&UDPServer::processPacket, this);

    while (running) {
        // Processamento adicional pode ser feito aqui
    }

    processPacketThread.join();
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
            processingBuffer.push({clientAddress, buffer});

        }
    }
}

void UDPServer::processPacket() {
    while (running) {
        std::lock_guard<std::mutex> lock(mutex);
        if (!processingBuffer.empty()) {
            std::pair<const sockaddr_in&, const std::vector<char>&> bufferValue = processingBuffer.front();
            processingBuffer.pop();
            const sockaddr_in& clientAddress = bufferValue.first;
            std::vector<char> packet = bufferValue.second;
            std::string returnMessage("        Exit request received");

            twt::Packet pack = twt::deserializePacket(packet);

            switch (pack.type) {
                case twt::PacketType::Mensagem: {
                    std::pair<int, std::string> payload = twt::deserializePacketPayload(packet);
                    twt::Message msg;
                    twt::User user;
                    user.userId = payload.first;
                    msg.content = payload.second;
                    msg.sender = user;
                    broadcastMessage(msg);
                    returnMessage = "        Message request received\nSender ID: " + std::to_string(msg.sender.userId) + "\nMessage: " + msg.content;
                    break;
                }
                case twt::PacketType::Follow: {
                    std::pair<int, std::string> payload = twt::deserializeFollowPayload(packet);
                    int followerId = payload.first;
                    std::string username = payload.second;
                    returnMessage = "        Follow request received\nFollower ID: " + std::to_string(followerId) + "\nUsername: " + username;
                    break;
                }
                case twt::PacketType::Login: {
                    std::string username = twt::deserializeLoginPayload(packet);
                    handleLogin(clientAddress, username);
                    continue;
                    break;
                }
                case twt::PacketType::Exit: {
                    int accountId = twt::deserializeExitPayload(packet);
                    // Handle exit logic if needed
                    break;
                }
            }

            std::cout << returnMessage << std::endl;
            sendto(serverSocket, returnMessage.c_str(), returnMessage.length(), 0, (struct sockaddr*)&clientAddress, sizeof(clientAddress));

        } else {
            sleep(0.1);
        }
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
        sendto(serverSocket, replyMessage.c_str(), replyMessage.length(), 0, (struct sockaddr*)&clientAddress, sizeof(clientAddress));
    }
}

void UDPServer::broadcastMessage(const twt::Message& message) {
    std::lock_guard<std::mutex> lock(mutex);
    std::cout << "post done" << std::endl;
    messageBuffer.push(message);
    cv.notify_all();  // Notifica todos os threads bloqueados na condição
}

int main() {
    UDPServer udpServer(PORT);
    udpServer.start();

    return 0;
}
