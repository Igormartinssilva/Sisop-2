#include <iostream>
#include <cstring>
#include <cstdlib>
#include <queue>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 4000
#define BUFFER_SIZE 1024

struct MessageData {
  int userId;
  char text[BUFFER_SIZE];
  int textSize;
};

struct UserCreationData {
  // Fields for user creation
  char username[32];  // 31 characters plus null terminator
  // Additional fields for account creation if needed
};

std::queue<MessageData> messageQueue;
std::queue<UserCreationData> userCreationQueue;
pthread_mutex_t messageMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t userCreationMutex = PTHREAD_MUTEX_INITIALIZER;


int main() {
    int sockfd, newsockfd;
    socklen_t clilen;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in serv_addr, cli_addr;
    MessageData receivedMessage;

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
      std::cout << "Received message from User ID " << receivedMessage.userId
                << ": " << receivedMessage.text << std::endl;

      // Send an acknowledgment back to the client
      n = sendto(sockfd, &receivedMessage, sizeof(receivedMessage), 0,
                  (struct sockaddr *)&cli_addr, clilen);
      if (n < 0) {
        std::cerr << "Error sending acknowledgment" << std::endl;
      }
    }

    close(sockfd);

    while (true) {
      pthread_mutex_lock(&messageMutex);
      if (!messageQueue.empty()) {
        MessageData data = messageQueue.front();
        messageQueue.pop();
        pthread_mutex_unlock(&messageMutex);

        // Process regular message data
      } else {
        pthread_mutex_unlock(&messageMutex);
        // Sleep or perform other work while waiting for data
      }
}

    return 0;
}
