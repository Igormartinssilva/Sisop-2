#include "session.hpp"

Session::Session() : client() {
    ifstream file("host.cache");
    if (file.is_open()) {
        string buffer;
        getline(file, buffer);
        client.setServer(buffer.c_str());
    } else {
        // Handle file open failure
    }
}

void Session::sendMessage(Message msg) {
    struct Bitstream bitstream = msg.createMessageBitstream();
    client.sendMessage(bitstream);
}
