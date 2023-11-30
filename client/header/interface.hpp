#include "user.hpp"

#ifndef INTERFACE_H
#define INTERFACE_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <any>

class Parser {
public:
    Interface(const std::string& configFile);
    void printDescription(const std::string& interface, const std::string& key) const;
private:        
    void parseJSON(const std::string& jsonString);
    std::string extractValue(const std::string& json, const std::string& key);
    
    std::map<std::string, std::any> config;
}

class Interface {
public:
    Interface(int userId);

private:
    User user;

};

#endif // INTERFACE_H


class Interface{
    private:
        User user;
    	Parser parser("interface.json");
	public:
        Interface();
        Interface(int id);
        
        void mainMenu();
        void writingMenu();
        void readingMenu();
}
