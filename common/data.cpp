#include "header/data.hpp"
#include <iostream>
#include <cstdint>
#include <arpa/inet.h>
#include "header/data.hpp"
#include <iostream>
#include <cassert>
#define MAGIC_NUMBER 0
#include <cstdint>

namespace twt {

    std::vector<char> serializeMessagePayload(int senderId, const std::string& message) {
        assert(BUFFER_SIZE >= 8); // Ensure there is enough space for the header and payload

        std::vector<char> serializedData(BUFFER_SIZE, 0);

        // Serialize message payload
        uint16_t senderIdN = htons(senderId);
        std::memcpy(serializedData.data() + 0, &senderIdN, sizeof(senderIdN));
        std::memcpy(serializedData.data() + 2, message.c_str(), message.size());

        return serializedData;
    }

    std::pair<int, std::string> deserializeMessagePayload(const std::vector<char>& data) {
        assert(data.size() >= 8); // Ensure there is enough data to deserialize the header and payload

        Packet packet = deserializePacket(data);

        int senderId;
        std::memcpy(&senderId, packet.payload, sizeof(senderId));
        senderId = ntohs(senderId);

        std::string message(packet.payload + 2, sizeof(packet.payload) - 2);

        return std::make_pair(senderId, message);
    }

    std::vector<char> serializeFollowPayload(int followerId, const std::string& username) {
        assert(BUFFER_SIZE >= 8); // Ensure there is enough space for the header and payload

        std::vector<char> serializedData(BUFFER_SIZE);

        // Serialize follow payload
        uint16_t followerIdN = htons(followerId);
        std::memcpy(serializedData.data() + 0, &followerIdN, sizeof(followerIdN));
        std::memcpy(serializedData.data() + 2, username.c_str(), username.size());

        return serializedData;
    }

    std::pair<int, std::string> deserializeFollowPayload(const std::vector<char>& data) {
        assert(data.size() >= 8); // Ensure there is enough data to deserialize the header and payload

        Packet packet = deserializePacket(data);

        int followerId;
        std::memcpy(&followerId, packet.payload, sizeof(followerId));
        followerId = ntohs(followerId);

        std::string username(packet.payload + 2, sizeof(packet.payload - 2));

        return std::make_pair(followerId, username);
    }

    std::vector<char> serializeExitPayload(int accountId) {
        assert(BUFFER_SIZE >= 6); // Ensure there is enough space for the header and payload

        std::vector<char> serializedData(BUFFER_SIZE);

        // Serialize exit payload
        uint16_t accountIdN = htons(accountId);
        std::memcpy(serializedData.data(), &accountIdN, sizeof(accountIdN));

        return serializedData;
    }

    int deserializeExitPayload(const std::vector<char>& data) {
        assert(data.size() >= 6); // Ensure there is enough data to deserialize the header and payload

        Packet packet = deserializePacket(data);

        int accountId;
        std::memcpy(&accountId, packet.payload, sizeof(accountId));
        accountId = ntohs(accountId);

        return accountId;
    }

    std::vector<char> serializeLoginPayload(const std::string& username) {
        assert(BUFFER_SIZE >= 2); // Ensure there is enough space for the header

        std::vector<char> serializedData(BUFFER_SIZE);

        // Serialize login payload
        std::memcpy(serializedData.data(), username.c_str(), username.size());

        return serializedData;
    }

    std::string deserializeLoginPayload(const std::vector<char>& data) {
        assert(data.size() >= 2); // Ensure there is enough data to deserialize the header and payload
        Packet packet = deserializePacket(data);

        std::string username(packet.payload);

        return username;
    }

    std::vector<char> serializePacket(const Packet &pkg) {
        assert(BUFFER_SIZE >= 6); // Ensure there is enough space for the header

        std::vector<char> serializedData(BUFFER_SIZE, 0);

        // Convert to network byte order (big-endian)
        uint16_t typeN = htons(pkg.type);
        uint16_t seqNumN = htons(pkg.sequence_number);
        uint16_t timestampN = htons(pkg.timestamp);

        int index = 0;
        std::memcpy(serializedData.data() + index, &typeN,      sizeof(typeN));         index += sizeof(typeN);
        std::memcpy(serializedData.data() + index, &seqNumN,    sizeof(seqNumN));       index += sizeof(seqNumN);
        std::memcpy(serializedData.data() + index, &timestampN, sizeof(timestampN));    index += sizeof(timestampN);
        std::memcpy(serializedData.data() + index, pkg.payload, sizeof(pkg.payload));

        return serializedData;
    }

    Packet deserializePacket(const std::vector<char> &data) {
        assert(data.size() >= 6); // Ensure there is enough data to deserialize the header

        Packet pkg;

        int index = 0;
        std::memcpy(&pkg.type,              data.data() + index, sizeof(pkg.type));             index += sizeof(pkg.type);
        std::memcpy(&pkg.sequence_number,   data.data() + index, sizeof(pkg.sequence_number));  index += sizeof(pkg.sequence_number);
        std::memcpy(&pkg.timestamp,         data.data() + index, sizeof(pkg.timestamp));        index += sizeof(pkg.timestamp);

        // Convert back to host byte order
        pkg.type = ntohs(pkg.type);
        pkg.sequence_number = ntohs(pkg.sequence_number);
        pkg.timestamp = ntohs(pkg.timestamp);

        std::memcpy(pkg.payload,            data.data() + index, sizeof(pkg.payload));

        return pkg;
    }
}


void twt::Followers::follow(int followerId, int followingId){
    followers[followingId].insert(followerId);
}

std::unordered_set<int> twt::Followers::getFollowers(int userId){
    return this->followers[userId];
}

  bool twt::Followers::isFollowing(int followerId, int followingId) {
        auto it = followers.find(followingId);
        if (it != followers.end()) {
            return it->second.find(followerId) != it->second.end();
        }
        return false;
    }

twt::UsersList::UsersList(){
    nextId = 1;
}

int twt::UsersList::appendUser(std::string username){
    users.insert({nextId, UserInfo(nextId, username)});
    usersId.insert({username, nextId});
    int returnId = nextId;
    this->nextId ++;
    return returnId;
}

int twt::UsersList::getUserId(std::string username){
    for (std::pair<const std::string, int> id : usersId) 
        if (std::strcmp(id.first.c_str(), username.c_str()) == 0)
            return id.second;
    return -1;
}

void twt::UsersList::removeUser(int userId){
    users.erase(userId);
}

int twt::UsersList::createSession(std::string username){
    int id = this->getUserId(username);
    if (id == -1){
        id = this->appendUser(username);
        std::cout << "user created: " << username << " with ID: " << id << std::endl;
        users[id].createSession();
        return id;
    } else if (!users[id].maxSessionsReached()){
        users[id].createSession();
        return id;
    } else {
        std::cout << "user " << username << " cannot login. Max session reached" << std::endl;
        return -1;
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

int twt::UserInfo::getId(){
    return this->user.userId;
}

std::string twt::UserInfo::getUsername(){
    return user.username;
}

bool twt::UserInfo::maxSessionsReached(){
    return activeSessions <= MAX_SESSIONS;
}

void twt::UserInfo::createSession(){
    this->activeSessions ++;
}
