#include <bits/stdc++.h>

class ServerNotification{
	public:
		int notificationId;
		int userPostId;
		char message;
		int messageSize;
		int followersToSend;
};

class UserNotification{
	public:
		int userPostId;
		int userReceiverId;
		int notificationId;
};

class FollowersGraph{
	private:
		std::vector<vector<int>> graph;
	public:
		void follow(int followerId, int followingId);
		void getFollowers(int userId);
}

class UsersList{
	private:
		int nextId;
		std::vector<vector<UserInfo>> users;
	public:
		void appendUser(char username);
		void removeUser(char username);
}
