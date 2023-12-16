#ifndef DATA_HPP
#define DATA_HPP

#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <cstring>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <cstdint>
#include <sstream>
#include <semaphore.h>
#include "../../asserts/constraints.hpp"

namespace twt{

	struct Packet {
		uint16_t type;
		uint16_t sequence_number;
		uint16_t timestamp;
		char payload[MSG_SIZE];
	};

	enum PacketType : uint16_t {
		Mensagem = 0,
		Login,
		Follow,
		Exit,
		Ping
	};

    // Serialization and Deserialization functions for different payloads
    std::vector<char> serializeMessagePayload(int senderId, const std::string& message);
    std::pair<int, std::string> deserializeMessagePayload(const std::vector<char>& data);



    std::vector<char> serializeFollowPayload(int followerId, const std::string& username);
    std::pair<int, std::string> deserializeFollowPayload(const std::vector<char>& data);

    std::vector<char> serializeExitPayload(int accountId);
    int deserializeExitPayload(const std::vector<char>& data);

	std::vector<char> serializePingPayload(int accountId);
    int deserializePingPayload(const std::vector<char>& data);

    std::vector<char> serializeLoginPayload(const std::string& username);
    std::string deserializeLoginPayload(const std::vector<char>& data);


	std::vector<char> serializePacket(const twt::Packet &pkg);
	twt::Packet deserializePacket(const std::vector<char> &data);

	struct User{
		std::string username;
		int userId;
	};

	class Followers{
		private:
			std::unordered_map<int, std::unordered_set<int>> followers;
		public:
			void follow(int followerId, int followingId);
			std::unordered_set<int> getFollowers(int userId);
			bool isFollowing(int followerId, int followingId);
			std::unordered_map<int, std::unordered_set<int>> getFollowersList();
			
	};

	class UserInfo {
		private:
			User user;
			int activeSessions;
			std::unordered_set<int> followers;
		public:
			UserInfo();
			UserInfo(int userId, std::string username);
			UserInfo(int userId, std::string username, std::unordered_set<int> followers);
			std::string getUsername();
			std::unordered_set<int> getFollowers();
			void addToFollowers(int id);
			void logout();
			int getId();
			bool maxSessionsReached();
			void createSession();
			void display();
	};

	class UsersList{
		private:
			int nextId;
			std::unordered_map<int, UserInfo> users;
			std::unordered_map<std::string, int> usersId;
		public:
			void setNextId(int nxtId);
			UsersList();
			void logout(int userId);
			int getUserId(std::string username);
			int appendUser(std::string username);
			std::string getUsername(int userId);
			void removeUser(int userId);
			int createSession(std::string username);
			void follow(int followerId, int followedId);

			std::unordered_map<int, UserInfo> getUserListInfo();	
			twt::UserInfo& getUser(int id);
			std::vector<int> getUserIds();

			std::vector<twt::UserInfo> storageMap();
			void loadMap(std::vector<twt::UserInfo>& users_list);
	};

	struct Message {
		User sender;
		std::string content;
	};

	extern sem_t sessionSemaphore;
	void initializeSemaphores();
    void destroySemaphores();
}

#endif