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

void Client::sendMessage(struct Bitstream bitstream) {
    int n;
    // Send the bitstream to the server
    n = sendto(sockfd, &bitstream, sizeof(bitstream), 0,
           (const struct sockaddr *)&serv_addr, sizeof(serv_addr));

    if (n < 0) {
        perror("ERROR in sendto");
        std::cerr << "Error code: " << errno << std::endl;
    }
    // Receive an acknowledgment into a temporary buffer
    struct Bitstream ack;
    n = recvfrom(sockfd, &ack, sizeof(Bitstream), 0, nullptr, nullptr);
    if (n < 0){
        perror("ERROR recvfrom");
        std::cerr << "Error code: " << errno << std::endl;
    }
    // Print the acknowledgment
    std::cout << "Got an ack: " << ack.message << std::endl;
}