#include "header/utils.hpp"
#include <cstdlib>

std::string getCurrentDateTime() {
    time_t now = time(0);
    tm *ltm = localtime(&now);

    // Formata a data e hora como uma string
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", ltm);

    return buffer;
}

bool nameConsistency(std::string name){
    return (
        (name.length() >= 4 && name.length() <= 20) &&
        name.find(',') == std::string::npos &&
        name.find(';') == std::string::npos
    );
}
