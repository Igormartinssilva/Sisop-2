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