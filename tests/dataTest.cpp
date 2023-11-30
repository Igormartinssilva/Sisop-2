#include "../common/header/data.hpp"
#include <iostream>

int main() {
    // Test the data structures and classes defined in the header file

    // Create a Package
    twt::Package package;
    package.type = twt::PackageType::Mensagem;
    package.sequence_number = 1;
    package.timestamp = 12345;
    package.payload = "Hello, world!";

    // Create a User
    twt::User user;
    user.username = "JohnDoe";
    user.userId = 42;

    // Create a UserInfo
    twt::UserInfo userInfo(1, "Alice");

    // Create a Followers object
    twt::Followers followers;
    followers.follow(1, 2);

    // Create a UsersList object
    twt::UsersList usersList;
    usersList.appendUser("Bob");
    int sessionId = usersList.createSession("Alice");

    // Display some information for testing purposes
    std::cout << "Package Payload: " << package.payload << std::endl;
    std::cout << "User ID: " << user.userId << ", Username: " << user.username << std::endl;
    std::cout << "User Info Username: " << userInfo.getUsername() << std::endl;
    std::cout << "Followers of user 1: " << followers.getFollowers(1).size() << std::endl;
    std::cout << "Session ID for Alice: " << sessionId << std::endl;

    return 0;
}
