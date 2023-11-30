#include "header/user.hpp"

using namespace std;

User::User(){
	this->userId = 0;
	this->username = "root"; // TODO: system.getUserInfo(this->userId);
}

User::User(int id){
	this->userId = id;
	this->username = ""; // TODO: system.getUserInfo(this->userId);
}

int User::getUserId(){
	return userId;
}