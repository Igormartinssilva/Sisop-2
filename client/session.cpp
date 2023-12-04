#include "header/session.hpp"

Session::Session() : client() {
    std::ifstream file("ip.txt"); // TODO: generalize to FILE_IP define directive
    if (file.is_open()) {
        std::string buffer;
        getline(file, buffer);
        client.setServer(buffer.c_str());
    } else {
        client.setServer("172.17.0.1"); //143.54.50.200 (dick) 143.54.48.125(luis)
    }
}

void Session::sendLogin(const std::string& username) {
    client.sendLogin(username);
}

void Session::sendFollow(int followerId, const std::string& username) {
    client.sendFollow(followerId, username);
}

void Session::sendMessage(const std::string& messageContent) {
    Message msg;
    msg.userId = user.getId();
    msg.message = messageContent;

    client.sendMessage(msg.userId, msg.message);
}

void Session::sendExit() {
    // Assuming user information is stored in the 'user' member variable
    client.sendExit(user.getId());
}