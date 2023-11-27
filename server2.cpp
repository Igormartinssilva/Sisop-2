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
#include "data.hpp"
#include "constraints.hpp"


class UDPServer {
public:
    UDPServer(int port);
    ~UDPServer();
    void start();

private:
    void handleMessages();
    void processMessage(const sockaddr_in& clientAddress, const std::string& message);
    void handleLogin(const sockaddr_in& clientAddress, const std::string& username);
    void handleReadRequest(const sockaddr_in& clientAddress, const std::string& username);
    void broadcastMessage(const twt::Message& message);

    int serverSocket;
    twt::Followers followers;
    twt::UsersList usersList;
    std::queue<twt::Message> messageBuffer;
    std::mutex mutex;
    std::condition_variable cv;
    bool running;
    std::unordered_map<int, std::queue<std::string>> messageBuffer;
};

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

    while (running) {
        // Processamento adicional pode ser feito aqui
    }

    messageThread.join();
}

void UDPServer::handleMessages() {
    while (running) {
        sockaddr_in clientAddress;
        socklen_t clientSize = sizeof(clientAddress);

        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));

        int bytesRead = recvfrom(serverSocket, buffer, sizeof(buffer), 0, (struct sockaddr*)&clientAddress, &clientSize);

        if (bytesRead > 0) {
            std::string message(buffer);
            processMessage(clientAddress, message);
        }
    }
}

void UDPServer::processMessage(const sockaddr_in& clientAddress, const std::string& message) {
    std::string command, content;
    size_t pos = message.find(' ');

    if (pos != std::string::npos) {
        command = message.substr(0, pos);
        content = message.substr(pos + 1);
    } else {
        command = message;
    }

    if (command == "LOGIN") {
        handleLogin(clientAddress, content);
    } else if (command == "READ") {
        handleReadRequest(clientAddress, content);
    } else if (command == "POST") {
        broadcastMessage({{"username", 15}, content});
    } else {
        // Lógica para processar outros comandos
    }

    

}

void UDPServer::handleLogin(const sockaddr_in& clientAddress, const std::string& username) {
    std::lock_guard<std::mutex> lock(mutex);
    bool id = usersList.createSession(username);

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
    int userId = std::stoi(userIdStr);

    std::lock_guard<std::mutex> lock(mutex);

    // Lógica para recuperar mensagens do buffer para o usuário específico e enviar de volta
    // ...
    std::queue<std::string>& userMessages = messageBuffer[userId];
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
    messageBuffer.push(message);
    cv.notify_all();  // Notifica todos os threads bloqueados na condição
}

int main() {
    UDPServer udpServer(PORT);
    udpServer.start();

    return 0;
}
