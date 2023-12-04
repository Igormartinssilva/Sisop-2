#include "header/client.hpp"

Client::Client() {
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
    return sendPackage(twt::MessageType::Login, payload);
}

void Client::sendFollow(int followerId, const std::string& username) {
    std::vector<char> payload = twt::serializeFollowPayload(followerId, username);
    sendPackage(twt::MessageType::Follow, payload);
}

void Client::sendMessage(int senderId, const std::string& message) {
    std::vector<char> payload = twt::serializeMessagePayload(senderId, message);
    sendPackage(twt::MessageType::Mensagem, payload);
}

void Client::sendExit(int accountId) {
    std::vector<char> payload = twt::serializeExitPayload(accountId);
    sendPackage(twt::MessageType::Exit, payload);
}

int Client::sendPackage(twt::MessageType type, const std::vector<char>& payload) {
    twt::Package package;
    package.type = static_cast<uint16_t>(type);
    package.sequence_number = 0; // You may set a meaningful sequence number here
    package.timestamp = 0; // You may set a meaningful timestamp here
    std::memcpy(package.payload, payload.data(), std::min(sizeof(package.payload), payload.size()));

    int n;
    // Send the bitstream to the server
    n = sendto(sockfd, &package, sizeof(twt::Package), 0,
               (const struct sockaddr *)&serv_addr, sizeof(serv_addr));

    if (n < 0) {
        perror("ERROR in sendto");
        std::cerr << "Error code: " << errno << std::endl;
    }
    // Receive an acknowledgment into a temporary buffer
    twt::Package ack;
    n = recvfrom(sockfd, &ack, sizeof(twt::Package), 0, nullptr, nullptr);
    if (n < 0) {
        perror("ERROR recvfrom");
        std::cerr << "Error code: " << errno << std::endl;
    }
    // Print the acknowledgment
    std::cout << "Got an ack: " << ack.payload << std::endl;
    return 1;
}
