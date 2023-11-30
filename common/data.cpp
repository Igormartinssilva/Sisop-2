#include "header/data.hpp"


std::string twt::serializePackage(const twt::Package &pkg) {
    std::ostringstream oss;
    oss.write(reinterpret_cast<const char *>(&pkg.type), sizeof(pkg.type));
    oss.write(reinterpret_cast<const char *>(&pkg.sequence_number), sizeof(pkg.sequence_number));
    oss.write(reinterpret_cast<const char *>(&pkg.timestamp), sizeof(pkg.timestamp));
    oss << pkg.payload;
    return oss.str();
}

twt::Package twt::deserializePackage(const std::string &data) {
    twt::Package pkg;
    std::istringstream iss(data);
    std::string buffer;
    iss.read(reinterpret_cast<char *>(&pkg.type), sizeof(pkg.type));
    iss.read(reinterpret_cast<char *>(&pkg.sequence_number), sizeof(pkg.sequence_number));
    iss.read(reinterpret_cast<char *>(&pkg.timestamp), sizeof(pkg.timestamp));
    std::getline(iss, buffer); 
    strcpy(pkg.payload, buffer.c_str());
    return pkg;
}

void twt::Followers::follow(int followerId, int followingId){
    followers[followingId].insert(followerId);
}

std::unordered_set<int> twt::Followers::getFollowers(int userId){
    return this->followers[userId];
}

void twt::UsersList::appendUser(std::string username){
    users.insert({nextId, UserInfo(nextId, username)});
    usersId.insert({username, nextId});
    this->nextId ++;
}

int twt::UsersList::getUserId(std::string username){
    if (usersId.find(username) != usersId.end())
        return usersId[username];
    else return -1;
}

void twt::UsersList::removeUser(int userId){
    users.erase(userId);
}

int twt::UsersList::createSession(std::string username){
    int id = this->getUserId(username);
    if (id == -1){
        this->appendUser(username);
        users[id].createSession();
        return id;
    }
    if (!users[id].maxSessionsReached()){
        users[id].createSession();
        return id;
    }
    return id;
}

twt::UserInfo::UserInfo(){
    this->user.username = "";
    this->user.userId = 0;
    this->activeSessions = 0;
}

twt::UserInfo::UserInfo(int userId, std::string username){
    this->user.username = username;
    this->user.userId = userId;
    this->activeSessions = 0;
}

std::string twt::UserInfo::getUsername(){
    return user.username;
}

bool twt::UserInfo::maxSessionsReached(){
    return activeSessions <= activeSessions;
}

void twt::UserInfo::createSession(){
    this->activeSessions ++;
}
