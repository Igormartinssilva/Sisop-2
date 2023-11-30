#include "header/message.hpp"

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

struct twt::Package Message::createMessageBitstream(){
    struct twt::Package package;
    time_t now = time(0);
    package.timestamp = now;
    if (message.size() < MSG_SIZE){
        package.type = twt::Mensagem;
        package.sequence_number = 0; // TODO: adicionar timestamp
        strcpy(package.payload, message.c_str());
    }
    else
        cerr << "Message greater than buffer size" << endl;
    return package;
}

