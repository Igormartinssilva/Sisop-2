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
#include <sstream>
#include "../../asserts/constraints.hpp"

namespace twt{

	struct Package {
		uint16_t type;
		uint16_t sequence_number;
		uint16_t timestamp;
		char payload[MSG_SIZE];
	};

	enum PackageType : uint16_t {
		Mensagem = 0,
		Login,
		Follow,
		Exit
	};

	std::vector<char> serializePackage(const twt::Package &pkg);
	twt::Package deserializePackage(const std::vector<char> &data);

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
	};

	class UserInfo {
		private:
			User user;
			int activeSessions;
		public:
			UserInfo();
			UserInfo(int userId, std::string username);
			std::string getUsername();
			bool maxSessionsReached();
			void createSession();
			
	};

	class UsersList{
		private:
			int nextId = 1;
			std::unordered_map<int, UserInfo> users;
			std::unordered_map<std::string, int> usersId;
			int getUserId(std::string username);
		public:
			void appendUser(std::string username);
			void removeUser(int userId);
			int createSession(std::string username);
	};

	struct Message {
		User sender;
		std::string content;
	};
}

#endif