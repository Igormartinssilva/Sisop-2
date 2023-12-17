#include "header/serialize.hpp"

namespace twt {

    std::string serializeMessagePayload(int senderId, const std::string& message) {
        std::stringstream serializedData;
        serializedData << senderId << ',' << message << '\0';
        
        return serializedData.str();
    }

    std::pair<int, std::string> deserializeMessagePayload(const std::string& data) {
        std::istringstream ss(data);
        
        int senderId;
        std::string message;
        
        ss >> senderId;
        std::getline(ss.ignore(), message);

        return std::make_pair(senderId, message);
    }

    std::string serializeFollowPayload(int followerId, const std::string& username) {
        std::stringstream serializedData;
        serializedData << followerId << ',' << username << '\0';
        
        return serializedData.str();
    }

    std::pair<int, std::string> deserializeFollowPayload(const std::string& data) {
        std::istringstream ss(data);
        
        int followerId;
        std::string username;
        
        ss >> followerId;
        std::getline(ss.ignore(), username);

        return std::make_pair(followerId, username);
    }

    std::string serializeExitPayload(int accountId) {
        std::stringstream serializedData;
        serializedData << accountId << '\0';
        
        return serializedData.str();
    }

    int deserializeExitPayload(const std::string& data) {
        std::istringstream ss(data);
        
        int accountId;
        ss >> accountId;

        return accountId;
    }

    std::string serializeLoginPayload(const std::string& username) {
        std::stringstream serializedData;
        serializedData << username << '\0';
        
        return serializedData.str();
    }

    std::string deserializeLoginPayload(const std::string& data) {
        return data;
    }

    std::string serializePingPayload(int accountId) {
        std::stringstream serializedData;
        serializedData << accountId << '\0';
        
        return serializedData.str();
    }

    int deserializePingPayload(const std::string& data) {
        std::istringstream ss(data);
        
        int accountId;
        ss >> accountId;

        return accountId;
    }

    std::string serializePacket(const Packet &pkg) {
        std::stringstream serializedData;
        serializedData << pkg.type << ',' << pkg.sequence_number << ',' << pkg.timestamp << ',' << pkg.payload << '\0';
        
        return serializedData.str();
    }

    Packet deserializePacket(const std::string &data) {
        std::istringstream ss(data);
        
        Packet pkg;

        ss >> pkg.type;
        ss.ignore(); // Ignore the comma
        ss >> pkg.sequence_number;
        ss.ignore(); // Ignore the comma
        ss >> pkg.timestamp;
        ss.ignore(); // Ignore the comma
        ss.getline(pkg.payload, BUFFER_SIZE - 6, '\0');

        return pkg;
    }

    
}