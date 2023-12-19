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
    for (std::pair<const std::string, int> id : usersId) {
       // std::cout << "username: " << username << std::endl;
       // std::cout << "id.first: " << id.first << std::endl;
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

void twt::UsersList:: loadMap(std::vector<twt::UserInfo>& users_list) {
    for (auto& user : users_list) {
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
    this->followers = followers;
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

void twt::UserInfo::addToFollowers(int id){
    followers.insert(id);
}

// Função para criar uma sessão para um usuário
int twt:: UsersList::createSession(std::string username) {
    int id = getUserId(username);
    if (id == -1) {
        id = appendUser(username);
        users[id].createSession();
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
 
void twt::UsersList::follow(int followerId, int followedId){
    users[followedId].addToFollowers(followerId);
}

// Função para fazer logout de uma sessão de um usuário
void twt::UsersList::logout(int userId) {
    users[userId].logout();

    // Após o logout, liberamos o semáforo para permitir que outro usuário se conecte
    sem_post(&twt::sessionSemaphore);
}
