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
        client.setServer("143.54.53.113"); //143.54.50.200 (dick) 143.54.48.125(luis) 172.25.219.12(igor) 143.54.53.113 (carlos)
    }
}

Session::Session(std::string ip) : client() {
    this->running = true;
    client.setServer(ip.c_str());
}

bool Session::isLogged(){
    return this->logged;
}

void Session::sendLogin(const std::string& username) {
    int id = client.sendLogin(username);
    if (id != -1){
        twt::User user;
        user.userId = id;
        user.username = username;
        this->user = user;
        this->logged = true;    
    }
    else
        this->logged = false;
}

void Session::sendFollow(const std::string& username) {
    if (this->user.userId == 0)
        return;
    client.sendFollow(this->user.userId, username);
}

void Session::sendMessage(const std::string& messageContent) {
    if (this->user.userId == 0)
        return;
    client.sendMessage(this->user.userId, messageContent);
}

void Session::sendExit() {
    if (this->user.userId == 0)
        return;
    client.sendExit(this->user.userId);
}

void Session::processReceiving() {
    while (running) {
        sockaddr_in serverAddress;
        socklen_t serverSize = sizeof(serverAddress);

        char buffer[BUFFER_SIZE];
        memset(buffer, 0, BUFFER_SIZE);

        int bytesRead = recvfrom(clientSocket, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&serverAddress, &serverSize);
        if (bytesRead > 0) 
        {            
            std::lock_guard<std::mutex> lock(mutex);
            receivingBuffer.push({serverAddress, buffer});
            std::string str(buffer);
            int index = str.find(',');
            std::string username = str.substr(0, index);
            int newIndex = str.find(',', index+1);
            int userId = atoi(str.substr(index+1, newIndex).c_str());
            std::string content = str.substr(newIndex+1);
            notificationBuffer.push({{username, userId}, content});
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

void Session::processNotifBuffer() {
    while(!notificationBuffer.empty()) {
        twt::Message msg = notificationBuffer.front();
        std::cout << "New Message from " << msg.sender.username << ": " << std::endl;
        std::cout << msg.content << std::endl;
        notificationBuffer.pop();
    }
}