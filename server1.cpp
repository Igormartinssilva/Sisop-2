#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 4000
#define BUFFER_SIZE 256

#define RED_TEXT "\033[1;31m"
#define GREEN_TEXT "\033[1;32m"
#define RESET_TEXT "\033[0m"

struct Message {
    int userId;
    char text[BUFFER_SIZE];
    int textSize;
};

int main() {
    int sockfd, newsockfd;
    socklen_t clilen;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in serv_addr, cli_addr;
    Message receivedMessage;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        std::cerr << "Error opening socket" << std::endl;
        exit(EXIT_FAILURE);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
        std::cerr << "Error on binding" << std::endl;
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    std::cout << "Server is listening on port " << PORT << std::endl;

    while (true) {
        clilen = sizeof(cli_addr);

        // Receive data from the client
        ssize_t n = recvfrom(sockfd, &receivedMessage, sizeof(receivedMessage), 0,
                             (struct sockaddr *)&cli_addr, &clilen);
        if (n < 0) {
            std::cerr << "Error receiving data" << std::endl;
            continue;
        }

        // Print the message along with the user ID
        std::cout << RED_TEXT << "Received message from User ID " << receivedMessage.userId
          << ": " << RESET_TEXT << GREEN_TEXT << receivedMessage.text << RESET_TEXT << std::endl;


        // Send an acknowledgment back to the client
        n = sendto(sockfd, &receivedMessage, sizeof(receivedMessage), 0,
                   (struct sockaddr *)&cli_addr, clilen);
        if (n < 0) {
            std::cerr << "Error sending acknowledgment" << std::endl;
        }
    }

    close(sockfd);

    return 0;
}
