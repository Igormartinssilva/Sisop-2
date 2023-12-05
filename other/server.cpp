#include <iostream>
#include <cstring>
#include <cstdlib>
#include <queue>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>  // Include the pthread library for thread functions

#define PORT 4000
#define BUFFER_SIZE 1024
#define NAME_SIZE 32

struct MessageData {
    int userId;
    char text[BUFFER_SIZE];
    int textSize;
};

struct UserCreationData {
    char username[32];
};

pthread_t messageThreadId, userCreationThreadId;
std::queue<MessageData> messageQueue;
std::queue<UserCreationData> userCreationQueue;
pthread_mutex_t messageMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t userCreationMutex = PTHREAD_MUTEX_INITIALIZER;

// Thread function for handling regular messages
void *messageThread(void *arg) {
    while (true) {
        pthread_mutex_lock(&messageMutex);
        if (!messageQueue.empty()) {
            MessageData data = messageQueue.front();
            messageQueue.pop();
            pthread_mutex_unlock(&messageMutex);

            // Process regular message data
            std::cout << "Received regular message from User ID " << data.userId << ": " << data.text << std::endl;
        } else {
            pthread_mutex_unlock(&messageMutex);
            // Sleep or perform other work while waiting for data
            sleep(1);
        }
    }
}

// Thread function for handling user creation data
void *userCreationThread(void *arg) {
    while (true) {
        pthread_mutex_lock(&userCreationMutex);
        if (!userCreationQueue.empty()) {
            UserCreationData data = userCreationQueue.front();
            userCreationQueue.pop();
            pthread_mutex_unlock(&userCreationMutex);

            // Process user creation data
            std::cout << "Received user creation data: Username - " << data.username << std::endl;
        } else {
            pthread_mutex_unlock(&userCreationMutex);
            // Sleep or perform other work while waiting for data
            sleep(1);
        }
    }
}

int main() {
    int sockfd;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;
    MessageData receivedMessageData;
    UserCreationData receivedUserCreationData;

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

    // Create threads
    pthread_create(&messageThreadId, nullptr, messageThread, nullptr);
    pthread_create(&userCreationThreadId, nullptr, userCreationThread, nullptr);

    while (true) {
        clilen = sizeof(cli_addr);

        // Receive regular message data from the client
        ssize_t n = recvfrom(sockfd, &receivedMessageData, sizeof(receivedMessageData), 0,
                              (struct sockaddr *)&cli_addr, &clilen);
        if (n < 0) {
            std::cerr << "Error receiving regular message data" << std::endl;
        }

        // Receive user creation data from the client
        n = recvfrom(sockfd, &receivedUserCreationData, sizeof(receivedUserCreationData), 0,
                      (struct sockaddr *)&cli_addr, &clilen);
        if (n < 0) {
            std::cerr << "Error receiving user creation data" << std::endl;
        }

        // Enqueue regular message data
        pthread_mutex_lock(&messageMutex);
        messageQueue.push(receivedMessageData);
        pthread_mutex_unlock(&messageMutex);

        // Enqueue user creation data
        pthread_mutex_lock(&userCreationMutex);
        userCreationQueue.push(receivedUserCreationData);
        pthread_mutex_unlock(&userCreationMutex);

        // Send an acknowledgment back to the client (for both types of data)
        n = sendto(sockfd, &receivedMessageData, sizeof(receivedMessageData), 0,
                    (struct sockaddr *)&cli_addr, clilen);
        if (n < 0) {
            std::cerr << "Error sending acknowledgment" << std::endl;
        }
    }

    // Wait for threads to finish (if needed)
    pthread_join(messageThreadId, nullptr);
    pthread_join(userCreationThreadId, nullptr);

    return 0;
}
