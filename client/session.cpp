#include "header/session.hpp"

Session::Session() : client() {
    std::ifstream file; // TODO: generalize to FILE_IP define directive
    file.open("../asserts/ip.txt");
    this->running = true;
    if (file.is_open()) {
        std::string buffer;
        getline(file, buffer);  
        std::cout << "ip found in file, ip: " << buffer << std::endl;
        client.setServer(buffer.c_str());
    } else {
        client.setServer("143.54.50.86"); //143.54.50.200 (dick) 143.54.48.125(luis) 172.25.219.12(igor)
    }
}

void Session::sendLogin(const std::string& username) {
    int id = client.sendLogin(username);
    this->user = twt::UserInfo(id, username);
}

void Session::sendFollow(const std::string& username) {
    if (this->user.getId() == 0)
        return;
    client.sendFollow(this->user.getId(), username);
}

void Session::sendMessage(const std::string& messageContent) {
    if (this->user.getId() == 0)
        return;
    client.sendMessage(this->user.getId(), messageContent);
}

void Session::sendExit() {
    if (this->user.getId() == 0)
        return;
    client.sendExit(this->user.getId());
}

void Session::processReceiving() {
    while (running) {
        sockaddr_in serverAddress;
        socklen_t serverSize = sizeof(serverAddress);

        char buffer[BUFFER_SIZE];
        memset(buffer, 0, BUFFER_SIZE);

        int bytesRead = recvfrom(clientSocket, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&serverAddress, &serverSize);
        if (bytesRead > 0) {            
            std::lock_guard<std::mutex> lock(mutex);
            receivingBuffer.push({serverAddress, buffer});

        }
    }
}

void Session::processBuffer() {
    while (running) {
        std::lock_guard<std::mutex> lock(mutex);
        if (!receivingBuffer.empty()) {
            std::pair<const sockaddr_in&, const std::string&> bufferValue = receivingBuffer.front();
            const sockaddr_in& serverAddress = bufferValue.first;
            std::string packet = bufferValue.second;
            receivingBuffer.pop();
            char receiveAck[BUFFER_SIZE];
            memset(receiveAck, 0, BUFFER_SIZE);
            strcpy(receiveAck, "Message Received");

            sendto(clientSocket, receiveAck, BUFFER_SIZE, 0, (struct sockaddr*)&serverAddress, sizeof(serverAddress));

        } else {
            sleep(0.1);
        }
    }
}