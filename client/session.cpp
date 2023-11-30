#include "header/session.hpp"

Session::Session() : client() {
    ifstream file("ip.txt"); // TODO: generalize to FILE_IP define directive
    if (file.is_open()) {
        string buffer;
        getline(file, buffer);
        cout << "Hello" << endl;
        cout << buffer << endl;
        client.setServer(buffer.c_str());
    } else {
        client.setServer("143.54.48.125");
    }
}

void Session::sendMessage(Message msg) {
    struct Bitstream bitstream = msg.createMessageBitstream();
    client.sendMessage(bitstream);
}
