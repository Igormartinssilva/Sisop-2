#include "message.hpp"

using namespace std;

Message::Message(int userId, string message){
    this->userId = userId;
    this->message = message;
    this->size = message.size();
}

Message::Message(){
    this->userId = 0;
    this->message = "";
    this->size = 0;
}

struct Bitstream Message::createMessageBitstream(){
    struct Bitstream bitstream;
    bitstream.size = message.size();
    bitstream.userId = userId;
    if (message.size() < BUFFER_SIZE)
        strcpy(bitstream.message, message.c_str());
    else
        cerr << "Message greater than buffer size" << endl;
    return bitstream;
}

