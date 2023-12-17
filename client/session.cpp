#include "header/session.hpp"
#include "../common/header/utils.hpp"

Session::Session() : client() {
    std::ifstream file; // TODO: generalize to FILE_IP define directive
    file.open("assets/ip.txt");
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

Session::~Session(){
    processingThread.join();
}

bool Session::isLogged(){
    return this->logged;
}

int Session::sendLogin(const std::string& username) {
    int retransmitAttempts = 0, nacks = 0;
    uint16_t timestamp = getTimeStamp();
    user.username = username;
    std::string payload = twt::serializeLoginPayload(username);
    do {
        retransmitAttempts++;
        std::cout << "Tentativa de transmissão: " << retransmitAttempts << std::endl;
        client.sendPacket(twt::PacketType::Login, timestamp, payload);
        if (!packetTransmited()) nacks++;
        else break;
    } while (nacks < 3 && retransmitAttempts < 3);
    if (nacks == 3) {
        std::cout<< "Não foi possível se conectar ao servidor" << RED << RESET << std::endl;
        return 0;
    } 
    return 1;


}

void Session::sendFollow(const std::string& username) {
    if (!isLogged()) return;
    std::string payload = twt::serializeFollowPayload(user.userId, username);
    //std::cout << "user.userId no follow : " << user.userId << std::endl;
    transmitPacket(twt::PacketType::Follow, payload);
}

void Session::sendMessage(const std::string& message) {
    if (!isLogged()) return;
    std::string payload = twt::serializeMessagePayload(user.userId, message);
    transmitPacket(twt::PacketType::Mensagem, payload);
}

void Session::sendExit() {
    std::string payload = twt::serializeExitPayload(user.userId);
    transmitPacket(twt::PacketType::Exit, payload);
    logged = false;
    running = false;
}

void Session::processBuffer() {
    while (running) {
        std::string packet = client.getBuffer();
        if (strcmp(packet.c_str(), "") != 0){
            uint16_t timestamp = getTimeStamp();
            if (packet.substr(0, 3) == "ACK") {
                if (packet.substr(0, 7) == "ACK_LOG") {
                    int index = packet.find(',', 8);
                    int index1 = packet.find(',', index+1);
                    user.userId = atoi(packet.substr(8, index).c_str());
                    user.username = packet.substr(index+1, index1);
                    std::string serverMsg = packet.substr(index1+1);
                    if (user.userId != -1){
                        logged = true;
                    } else {
                        logged = false;
                    }
                    std::cout << PURPLE << "Server Message: " << RESET << serverMsg << std::endl;
                }
                else if (packet.substr(0, 7) == "ACK_FLW") {
                    std::cout << PURPLE << "Server Message: " << RESET << packet.substr(8) << std::endl;
                }
                else if (packet.substr(0, 7) == "ACK_MSG") {
                    std::cout << PURPLE << "Server Message: " << RESET << packet.substr(8) << std::endl;
                }
                else if (packet.substr(0, 7) == "ACK_EXT") {
                    std::cout << PURPLE << "Server Message: " << RESET << packet.substr(8) << std::endl;
                }
                ackReceived = true;
            } else if (packet.substr(0, 3) == "PNG") {
                client.sendPacket(twt::Ping, timestamp, twt::serializePingPayload(user.userId));
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

bool Session::packetTransmited() {
    auto start = std::chrono::high_resolution_clock::now();
    while (!ackReceived) {
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        if (elapsed.count() > 3.0) { // Se passaram mais de 3 segundos
            std::cout << "Ack não recebido, retransmita" << std::endl;
            return false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Dormir por 10 milissegundos para evitar uso excessivo de CPU
    }
    ackReceived = false;
    return true;
}

void Session::transmitPacket(twt::PacketType type, std::string payload) {
    uint16_t timestamp = getTimeStamp();
    int retransmitAttempts = 0, n, nacks = 0;
    if (!isLogged()) return;
    while (nacks < 3 && retransmitAttempts < 3)
    {
        retransmitAttempts++;
        n = client.sendPacket(type, timestamp, payload);
        if (!packetTransmited()) nacks++;
        else break;
    }

    if (n < 0) {
        perror("ERROR in sendto");
        std::cerr << "Error code: " << errno << std::endl;
    } 

    if (nacks == 3) std::cout<< "Não foi possível se conectar ao servidor, tente novamente dentro de alguns instantes" << RED << RESET << std::endl;
    
    return;
}
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