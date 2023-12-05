#include "header/data.hpp"
#include <iostream>

int main(){
    twt::Packet packet({0, 0, 0, "Hello"});
    std::vector<char> bitstream(twt::serializePacket(packet));
    std::cout << "size: " << bitstream.size() << std::endl;
    
    twt::Packet newPacket(twt::deserializePacket(bitstream));
    std::cout << "type_op: " << newPacket.type << std::endl;
    std::cout << "seq_num: " << newPacket.sequence_number << std::endl;
    std::cout << "timestp: " << newPacket.timestamp << std::endl;
    std::cout << "payload: " << newPacket.payload << std::endl;
    
    return 0;
}