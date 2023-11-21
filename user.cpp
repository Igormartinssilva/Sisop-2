#include "user.hpp"

using namespace std;

User::User(int id){
	this->userId = id;
	this->username = ""; // TODO: system.getUserInfo(this->userId);
}

void User::sendMessage(string message){
	// TODO: system.sendMessage(this->userId, message);
	
}

void User::loadTimeline(){
	// TODO: system.getTimeline(this->userId);

}
