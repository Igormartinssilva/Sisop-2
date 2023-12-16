#include "header/session.hpp"

Session::Session() : client() {
    std::ifstream file; // TODO: generalize to FILE_IP define directive
    file.open("../asserts/ip.txt");
    this->logged = false;
    this->running = true;
    if (file.is_open()) {
        std::string buffer;
        getline(file, buffer);  
        std::cout << "ip found in file, ip: " << buffer << std::endl;
        client.setServer(buffer.c_str());
    } else {
        client.setServer("143.54.53.113"); 
    }
    processingThread = std::thread(&Session::processBuffer, this);
}

Session::Session(std::string ip) : client() {
    this->running = true;
    this->logged = false;
    client.setServer(ip.c_str());
    processingThread = std::thread(&Session::processBuffer, this);
}

bool Session::isLogged(){
    return this->logged;
}

void Session::sendLogin(const std::string& username) {
    user.username = username;
    std::vector<char> payload = twt::serializeLoginPayload(username);
    client.sendPacket(twt::PacketType::Login, payload);
    std::cout << "waiting for acknowledgment..." << std::endl;
    std::cout << "user.userId (esquenta) : " << user.userId << std::endl;
    waitForAck();
    std::cout << "user.userId (after): " << user.userId << std::endl;
}

void Session::sendFollow(const std::string& username) {
    if (!isLogged()) return;
    std::vector<char> payload = twt::serializeFollowPayload(user.userId, username);
    std::cout << "user.userId no follow : " << user.userId << std::endl;
    client.sendPacket(twt::PacketType::Follow, payload);
    waitForAck();
}

void Session::sendMessage(const std::string& message) {
    if (!isLogged()) return;
    std::vector<char> payload = twt::serializeMessagePayload(user.userId, message);
    client.sendPacket(twt::PacketType::Mensagem, payload);
    waitForAck();
}

void Session::sendExit() {
    if (!isLogged()) return;
    std::vector<char> payload = twt::serializeExitPayload(user.userId);
    client.sendPacket(twt::PacketType::Exit, payload);
    waitForAck();
    logged = false;
    running = false;
}

void Session::processBuffer() {
    while (true) {
        std::string packet = client.getBuffer();
        if (strcmp(packet.c_str(), "") != 0){
            if (packet.substr(0, 3) == "ACK") {
                std::cout << "Packet: " << packet << std::endl;
                if (packet.substr(0, 7) == "ACK_LOG") {
                    int index = packet.find(',', 8);
                    user.userId = atoi(packet.substr(8, index).c_str());
                    logged = true;
                }
                ackReceived = true;
            } else {
                messageBuffer.push(packet);
            }
        }
        else {
            sleep(0.01);
        }
    }
}

std::string Session::getMessageBuffer() {
    std::lock_guard<std::mutex> lock(bufferMutex);

    if(messageBuffer.empty()) {
        return "";
    }

    std::string next = messageBuffer.front();
    messageBuffer.pop();
    return next;
}

void Session::waitForAck() {
    while (!ackReceived);
    ackReceived = false;
}

void Session::printYourMessages() {
    while (true) {
        std::string message = this->getMessageBuffer();
        if (message.empty()) {
            break;
        }
        std::cout << message << std::endl;
    }
}
