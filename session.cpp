#include "session.hpp"

void Session::sendMessage(Message msg){
    string bitstream = msg.createMessageBitstream();
    //TODO: send message via UDP
}