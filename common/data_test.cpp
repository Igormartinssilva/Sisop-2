#include "header/data.hpp"
#include <iostream>

int main(){
    twt::Package packet({0, 0, 0, "Hello"});
    std::vector<char> bitstream(twt::serializePackage(packet));
    std::cout << "size: " << bitstream.size() << std::endl;
    
    twt::Package newPackage(twt::deserializePackage(bitstream));
    std::cout << "type_op: " << newPackage.type << std::endl;
    std::cout << "seq_num: " << newPackage.sequence_number << std::endl;
    std::cout << "timestp: " << newPackage.timestamp << std::endl;
    std::cout << "payload: " << newPackage.payload << std::endl;
    
    return 0;
}