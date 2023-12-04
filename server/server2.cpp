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
    std::thread messageThread(&UDPServer::handleMessages, this);
    std::thread processMessageThread(&UDPServer::processMessage, this);

    while (running) {
        // Processamento adicional pode ser feito aqui
    }

    messageThread.join();
    processMessageThread.join();
}

void UDPServer::handleMessages() {
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

void UDPServer::processMessage() {
    while (running) {
        std::lock_guard<std::mutex> lock(mutex);
        if (!processingBuffer.empty()) {
            std::pair<const sockaddr_in&, const std::vector<char>&> bufferValue = processingBuffer.front();
            processingBuffer.pop();
            const sockaddr_in& clientAddress = bufferValue.first;
            std::vector<char> message = bufferValue.second;
            std::string returnMessage("Exit request received");

            twt::Package pack = twt::deserializePackage(message);

            switch (pack.type) {
                case twt::MessageType::Mensagem: {
                    auto [senderId, messageContent] = twt::deserializeMessagePayload(message);
                    returnMessage = "Message request received\nSender ID: " + std::to_string(senderId) + "\nMessage: " + messageContent;
                    break;
                }
                case twt::MessageType::Follow: {
                    auto [followerId, username] = twt::deserializeFollowPayload(message);
                    returnMessage = "Follow request received\nFollower ID: " + std::to_string(followerId) + "\nUsername: " + username;
                    break;
                }
                case twt::MessageType::Login: {
                    std::string username = twt::deserializeLoginPayload(message);
                    handleLogin(clientAddress, username);
                    continue;
                    break;
                }
                case twt::MessageType::Exit: {
                    int accountId = twt::deserializeExitPayload(message);
                    // Handle exit logic if needed
                    break;
                }
            }

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
    //messageBuffer.insert(message);
    cv.notify_all();  // Notifica todos os threads bloqueados na condição
}

int main() {
    UDPServer udpServer(PORT);
    udpServer.start();

    return 0;
}
