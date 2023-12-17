#pragma once
#include <string>
#include <vector>
#include "data.hpp"

namespace twt{
  std::string serializeMessagePayload(int senderId, const std::string& message);
	std::pair<int, std::string> deserializeMessagePayload(const std::string& data);

	std::string serializeFollowPayload(int followerId, const std::string& username);
	std::pair<int, std::string> deserializeFollowPayload(const std::string& data);

	std::string serializeExitPayload(int accountId);
	int deserializeExitPayload(const std::string& data);

	std::string serializePingPayload(int accountId);
	int deserializePingPayload(const std::string& data);

	std::string serializeLoginPayload(const std::string& username);
	std::string deserializeLoginPayload(const std::string& data);

	std::string serializePacket(const Packet& pkg);
	Packet deserializePacket(const std::string& data);
}