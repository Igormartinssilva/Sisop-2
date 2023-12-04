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
    int id = 1;//int id = client.sendLogin(username);
    this->user = twt::UserInfo(id, username);
}

void Session::sendFollow(const std::string& username) {
    if (this->user.getId() == 0)
        return;
    client.sendFollow(this->user.getId(), username);
}

void Session::sendMessage(const std::string& messageContent) {
    if (this->user.getId() == 0)
        return;
    client.sendMessage(this->user.getId(), messageContent);
}

void Session::sendExit() {
    if (this->user.getId() == 0)
        return;
    client.sendExit(this->user.getId());
}