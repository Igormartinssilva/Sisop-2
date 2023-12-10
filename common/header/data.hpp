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
#include "../../asserts/constraints.hpp"
#include "../../server/header/server.hpp"

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
		Exit
	};

    // Serialization and Deserialization functions for different payloads
    std::vector<char> serializeMessagePayload(int senderId, const std::string& message);
    std::pair<int, std::string> deserializeMessagePayload(const std::vector<char>& data);

    std::vector<char> serializeFollowPayload(int followerId, const std::string& username);
    std::pair<int, std::string> deserializeFollowPayload(const std::vector<char>& data);

    std::vector<char> serializeExitPayload(int accountId);
    int deserializeExitPayload(const std::vector<char>& data);

    std::vector<char> serializeLoginPayload(const std::string& username);
    std::string deserializeLoginPayload(const std::vector<char>& data);


	std::vector<char> serializePacket(const twt::Packet &pkg);
	twt::Packet deserializePacket(const std::vector<char> &data);

	struct ServerNotification{
		int notificationId;
		int userPostId;
		char message;
		int messageSize;
		int followersToSend;
	};

	struct UserNotification{
		int userPostId;
		int userReceiverId;
		int notificationId;
	};

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
			
	};

	class UserInfo {
		private:
			User user;
			int activeSessions;
			std::unordered_set<int> followers;
		public:
			UserInfo();
			UserInfo(int userId, std::string username, std::unordered_set<int> followers);
			std::string getUsername() const;
			int getId() const;
			void logout();
			bool maxSessionsReached();
			void createSession();
			const std::unordered_set<int> getFollowers() const;
			
	};

	class UsersList{
		private:
			int nextId;
			std::unordered_map<int, UserInfo> users;
			std::unordered_map<std::string, int> usersId;
		public:
			UsersList();
			void logout(int userId);
			int getUserId(std::string username);
			int appendUser(std::string username);
			void removeUser(int userId);
			int createSession(std::string username);
			std::vector<twt::UserInfo> mapToVector();
			std::unordered_map<int, UserInfo> vectorToMap(const std::vector<twt::UserInfo>& users);
			
	};

	struct Message {
		User sender;
		std::string content;
	};
}

#endif