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
    while (running){
        std::lock_guard<std::mutex> lock(mutex);
        if (!processingBuffer.empty()) {
            std::pair<const sockaddr_in&, const std::vector<char>&> bufferValue = processingBuffer.front();
            processingBuffer.pop();
            const sockaddr_in& clientAddress = bufferValue.first;
            std::vector<char> message = bufferValue.second;
            std::string returnMessage("Exit request received");
            
            
            twt::Package pack = twt::deserializePackage(message);
            switch(pack.type){
                case twt::Mensagem:
                    returnMessage = "Message request received";
                    break;
                case twt::Follow:
                    returnMessage = "Follow request received";
                    break;
                case twt::Login:
                    returnMessage = "Login request received";
                    break;
                case twt::Exit:
                    returnMessage = "Exit request received";
                    break;
            }
            std::cout << returnMessage << std::endl;
            sendto(serverSocket, &returnMessage, returnMessage.length(), 0, (struct sockaddr*)&clientAddress, sizeof(clientAddress));

            std::cout << "type: " << pack.type << std::endl;
            std::cout << "timestamp: " << pack.timestamp << std::endl;
            std::cout << "sequence number: " << pack.sequence_number << std::endl;
            std::cout << "payload: " << pack.payload << std::endl;
            
        }
        else{
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

void UDPServer::handleReadRequest(const sockaddr_in& clientAddress, const std::string& userIdStr) {
    //int userId = std::stoi(userIdStr);

    std::lock_guard<std::mutex> lock(mutex);

    // Lógica para recuperar mensagens do buffer para o usuário específico e enviar de volta
    // ...
    //std::queue<std::string>& userMessages = messageBuffer;
    // Exemplo de resposta
     // Enviar resposta de recebimento ao cliente
    std::string replyMessage = "Server received your message!";
    ssize_t sentBytes = sendto(serverSocket, replyMessage.c_str(), replyMessage.length(), 0, (struct sockaddr*)&clientAddress, sizeof(clientAddress));

    if( sentBytes == -1){
        perror("ERROR to sending message");
        //pode-se add outra trativas como encerrar conexao ou tentar reenviar a menssagem.
    }
    else{
        std::cout << "Sent " << sentBytes << " bytes to the client. \n";
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
