#include "header/client.hpp"

Client::Client() {
    srand(time(NULL));
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        std::cerr << "Error opening socket" << std::endl;
        exit(EXIT_FAILURE);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
}

Client::~Client() {
    close(sockfd);
}

void Client::setServer(const char *hostname) {
    struct hostent *server = gethostbyname(hostname);
    if (server == nullptr) {
        std::cerr << "ERROR, no such host" << std::endl;
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr = *((struct in_addr *)server->h_addr);
}

int Client::sendLogin(const std::string& username) {
    std::vector<char> payload = twt::serializeLoginPayload(username);
    return sendPacket(twt::PacketType::Login, payload);
}

void Client::sendFollow(int followerId, const std::string& username) {
    std::vector<char> payload = twt::serializeFollowPayload(followerId, username);
    sendPacket(twt::PacketType::Follow, payload);
}

void Client::sendMessage(int senderId, const std::string& message) {
    std::vector<char> payload = twt::serializeMessagePayload(senderId, message);
    sendPacket(twt::PacketType::Mensagem, payload);
}

void Client::sendExit(int accountId) {
    std::vector<char> payload = twt::serializeExitPayload(accountId);
    sendPacket(twt::PacketType::Exit, payload);
}

int Client::sendPacket(twt::PacketType type, const std::vector<char>& payload) {
    twt::Packet packet;
    packet.type = static_cast<uint16_t>(type);
    packet.sequence_number = 250; // You may set a meaningful sequence number here
    packet.timestamp = 150; // You may set a meaningful timestamp here
    std::memcpy(packet.payload, payload.data(), std::min(sizeof(packet.payload), payload.size()));
    std::vector<char> bitstream = twt::serializePacket(packet);

    char bits[BUFFER_SIZE];
    for (int i = 0; i < BUFFER_SIZE; i ++)
        bits[i] = bitstream[i];
    

    if (DEBUG_CLIENT){
        for (char ch : bitstream)
            std::cout << int(ch) << " ";
        std::cout << std::endl;
    }

    int n;

    // Send the bitstream to the server
    n = sendto(sockfd, &bits, BUFFER_SIZE, 0,
               (const struct sockaddr *)&serv_addr, sizeof(serv_addr));

    if (n < 0) {
        perror("ERROR in sendto");
        std::cerr << "Error code: " << errno << std::endl;
    }
    // Receive an acknowledgment into a temporary buffer
    char ack[BUFFER_SIZE];
    n = recvfrom(sockfd, &ack, BUFFER_SIZE, 0, nullptr, nullptr);
    if (n < 0) {
        perror("ERROR recvfrom");
        std::cerr << "Error code: " << errno << std::endl;
    }
    // Print the acknowledgment
    std::cout << "Got an ack: " << ack << std::endl;
    return rand();
}
