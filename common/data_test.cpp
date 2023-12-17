#include "header/data.hpp"
#include <cassert>
#include <iostream>

using namespace twt;

int main() {
    // Test serializeFollowPayload and deserializeFollowPayload
    std::string serializedFollowPayload = serializeFollowPayload(456, "john_doe");
    std::pair<int, std::string> deserializedFollowPayload = deserializeFollowPayload(serializedFollowPayload);
    assert(deserializedFollowPayload.first == 456);
    assert(deserializedFollowPayload.second == "john_doe");

    // Test serializeExitPayload and deserializeExitPayload
    std::string serializedExitPayload = serializeExitPayload(789);
    int deserializedExitPayload = deserializeExitPayload(serializedExitPayload);
    assert(deserializedExitPayload == 789);

    // Test serializeLoginPayload and deserializeLoginPayload
    std::string serializedLoginPayload = serializeLoginPayload("user123");
    std::string deserializedLoginPayload = deserializeLoginPayload(serializedLoginPayload);
    assert(deserializedLoginPayload == "user123");

    // Test serializePingPayload and deserializePingPayload
    std::string serializedPingPayload = serializePingPayload(999);
    int deserializedPingPayload = deserializePingPayload(serializedPingPayload);
    assert(deserializedPingPayload == 999);

    // Test serializePacket and deserializePacket
    Packet packet;
    packet.type = 1;
    packet.sequence_number = 42;
    packet.timestamp = 12345;
    std::strcpy(packet.payload, "Hello, Packet!");
    std::string serializedPacket = serializePacket(packet);
    Packet deserializedPacket = deserializePacket(serializedPacket);
    assert(deserializedPacket.type == 1);
    assert(deserializedPacket.sequence_number == 42);
    assert(deserializedPacket.timestamp == 12345);
    assert(std::strcmp(deserializedPacket.payload, "Hello, Packet!") == 0);

    std::cout << "All tests passed successfully!" << std::endl;

    return 0;
}