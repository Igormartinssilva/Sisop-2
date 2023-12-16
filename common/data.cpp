#include "header/data.hpp"
#include <iostream>
#include <cstdint>
#include <arpa/inet.h>
#include "header/data.hpp"
#include <iostream>
#include <cassert>
#define MAGIC_NUMBER 0
#include <cstdint>
#include <vector>
#include <unordered_set>

sem_t twt::sessionSemaphore;
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

// Definindo um semáforo global para controlar o número de sessões por usuário
    
 // Função para inicializar o semáforo no início do programa
void twt::initializeSemaphores() {
    // Inicialize o semáforo com um valor de 2 (duas sessões permitidas por vez)
    sem_init(&twt::sessionSemaphore, 0, 2);
}

    // Função para destruir o semáforo no final do programa
void twt::destroySemaphores() {
    sem_destroy(&twt::sessionSemaphore);
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

std::unordered_map<int, std::unordered_set<int>> twt::Followers::getFollowersList() {
    return this->followers;
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

void twt::UsersList::setNextId(int nxtId){
    this->nextId = nxtId;
}
/*
void twt::UsersList::logout(int userId){
    users[userId].logout();
}*/

int twt::UsersList::getUserId(std::string username){
    std::cout << "Length of usersId = " << this->usersId.size() << std::endl;
    for (std::pair<const std::string, int> id : usersId) {
        std::cout << "comparing " << id.first << " with " << username << std::endl;
        std::cout << "result = " << (std::strcmp(id.first.c_str(), username.c_str())) << std::endl;
        if (std::strcmp(id.first.c_str(), username.c_str()) == 0){
            return id.second;
        }
    }
    return -1;
}

std::string twt::UsersList::getUsername(int userId){
    return users[userId].getUsername();
}

void twt::UsersList::removeUser(int userId){
    users.erase(userId);
}

std::vector<int> twt::UsersList::getUserIds() {
        std::vector<int> userIds;
        for (const auto& pair : users) {
            userIds.push_back(pair.first);
        }
        return userIds;
    }

twt::UserInfo& twt::UsersList::getUser(int userId) {
        return users[userId];
    }

std::unordered_map<int, twt::UserInfo> twt::UsersList::getUserListInfo(){
    return this->users;
}

std::vector<twt::UserInfo> twt::UsersList::storageMap() {
    std::vector<twt::UserInfo> result;

    for (const auto& pair : this->users) {
        result.push_back(pair.second);  // Adiciona cada UserInfo ao vetor
    }

    return result;
}

void twt::UsersList::loadMap(std::vector<twt::UserInfo>& users_list) {
    for (auto& user : users_list) 
    {
        this->users[user.getId()] = user;  // Usa getId() como chave e insere no mapa
        this->usersId[user.getUsername()] = user.getId();
    }
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

twt::UserInfo::UserInfo(int userId, std::string username, std::unordered_set<int> followers){
    this->user.username = username;
    this->user.userId = userId;
    this->activeSessions = 0;
}

int twt::UserInfo::getId(){
    return this->user.userId;
}

void twt::UserInfo::logout(){
    if (activeSessions > 0)
        activeSessions --;
}


bool twt::UserInfo::maxSessionsReached(){
    return activeSessions >= MAX_SESSIONS;
}

void twt::UserInfo::createSession(){
    this->activeSessions ++;
}


std::string twt::UserInfo::getUsername(){
    return user.username;
}

std::unordered_set<int> twt::UserInfo::getFollowers() {
    return this->followers;            
}
constexpr char RED[] = "\033[1;31m";
constexpr char GREEN[] = "\033[1;32m";
constexpr char YELLOW[] = "\033[1;33m";
constexpr char BLUE[] = "\033[1;34m";
constexpr char RESET[] = "\033[0m";

void twt::UserInfo::display() {
    std::cout << "\033[1;34m" << getId() << "\033[0m" << "\t - @";
    std::cout << "\033[1;31m" << getUsername() << "\033[0m" << std::endl;
}



// Função para criar uma sessão para um usuário
int twt:: UsersList::createSession(std::string username) {
    int id = getUserId(username);
    std::cout << "inside create session: username = " << username << ", id = " << std::to_string(id) << std::endl;
    if (id == -1) {
        id = appendUser(username);
        std::cout << "\n> User created: " << username << " with ID: " << id << std::endl;
        users[id].createSession();
        std::cout << "\n> Creating session: " << username << " with ID: " << id << std::endl;
        return id;
    } else {
        // Agora, antes de criar uma nova sessão, esperamos o semáforo
        //sem_wait(&twt::sessionSemaphore);

        if (!users[id].maxSessionsReached()) {
            users[id].createSession();
            std::cout << "\n> Creating session: " << username << " with ID: " << id << std::endl;
            return id;
        } else {
            std::cout << "\n> User " << username << " cannot log in. Max session reached" << std::endl;

            // Se não for possível criar uma sessão, liberamos o semáforo
            //sem_post(&twt::sessionSemaphore);

            return -1;
        }
    }

    return id;
}
 
     // Função para fazer logout de uma sessão de um usuário
void twt::UsersList::logout(int userId) {
    users[userId].logout();

    // Após o logout, liberamos o semáforo para permitir que outro usuário se conecte
    sem_post(&twt::sessionSemaphore);
}
