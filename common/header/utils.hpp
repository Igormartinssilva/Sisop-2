#include <iostream>
#include <string>
#include <random>
#include <ctime>

constexpr char RED[] = "\033[1;31m";
constexpr char GREEN[] = "\033[1;32m";
constexpr char YELLOW[] = "\033[1;33m";
constexpr char BLUE[] = "\033[1;34m";
constexpr char PURPLE[] = "\033[1;35m";
constexpr char RESET[] = "\033[0m";

std::string getCurrentDateTime();
bool nameConsistency( std::string username);
void clearScreen();
void pressEnterToContinue();