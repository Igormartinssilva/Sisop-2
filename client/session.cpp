#include "header/session.hpp"

Session::Session() : client() {
    ifstream file("ip.txt"); // TODO: generalize to FILE_IP define directive
    if (file.is_open()) {
        string buffer;
        getline(file, buffer);
        client.setServer(buffer.c_str());
    } else {
        client.setServer("172.17.0.1"); //143.54.50.200 (dick) 143.54.48.125(luis)
    }
}

void Session::sendMessage(Message msg) {
    struct twt::Package packet = msg.createMessageBitstream();
    std::vector<char> bitstream = twt::serializePackage(packet);

    client.sendMessage(bitstream);
}
