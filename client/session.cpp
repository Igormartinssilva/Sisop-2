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
    waitForAck();
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
                if (packet.substr(0, 7) == "ACK_LOG") {
                    int index = packet.find(',', 8);
                    user.userId = atoi(packet.substr(8, index).c_str());
                    logged = true;
                }
                else if (packet.substr(0, 7) == "ACK_FLW") {
                    std::cout << packet.substr(8) << std::endl;
                }
                else if (packet.substr(0, 7) == "ACK_MSG") {
                    std::cout << packet.substr(8) << std::endl;
                }
                else if (packet.substr(0, 7) == "ACK_EXT") {
                    std::cout << packet.substr(8) << std::endl;
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

constexpr char RED[] = "\033[1;31m";
constexpr char GREEN[] = "\033[1;32m";
constexpr char YELLOW[] = "\033[1;33m";
constexpr char BLUE[] = "\033[1;34m";
constexpr char PURPLE[] = "\033[1;35m";
constexpr char RESET[] = "\033[0m";

void Session::printYourMessages() {
    while (true) {
        std::string message = this->getMessageBuffer();
        if (message.empty()) {
            break;
        }
        twt::Message msg = decodeMessage(message);
        std::cout << PURPLE << msg.sender.username << RESET << " @" << msg.sender.username << RESET << ":" << std::endl;
        std::cout << "> " << msg.content << std::endl << std::endl;
    }
}

twt::Message Session::decodeMessage(std::string str){
    twt::Message msg;
    int i0, i1;
    i0 = str.find(',');
    i1 = str.find(',', i0+1);
    msg.sender.username = str.substr(0, i0);
    msg.sender.userId = atoi(str.substr(i0+1, i1).c_str());
    msg.content = str.substr(i1+1);
    return msg;
}