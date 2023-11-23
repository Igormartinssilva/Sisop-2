#include "message.hpp"

using namespace std;

Message::Message(int userId, string message){
    this->user = User(userId);
    this->message = message;
    this->size = message.size();
}

Message::Message(){
    this->user = User();
    this->message = ""
    this->size = 0;
}

string createMessageBitstream(){
    // Calculate the size of the encoded data
    size_t encodedSize = sizeof(this->userId) + sizeof(uint32_t) + this->size();

    // Create a buffer to hold the encoded data
    char* buffer = new char[encodedSize];

    // Use memcpy to copy data into the buffer
    std::memcpy(buffer, &userId, sizeof(userId));
    
    uint32_t messageSize = static_cast<uint32_t>(message.size());
    std::memcpy(buffer + sizeof(userId), &messageSize, sizeof(messageSize));

    std::memcpy(buffer + sizeof(userId) + sizeof(messageSize), message.c_str(), message.size());

    // Create a string from the buffer
    std::string encodedData(buffer, encodedSize);

    // Clean up the allocated buffer
    delete[] buffer;

    return encodedData;
}

