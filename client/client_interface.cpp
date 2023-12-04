// main.cpp

#include "header/session.hpp"
#include <cstdlib> // for std::system

using namespace std;

// ANSI color codes
constexpr char RED[] = "\033[1;31m";
constexpr char GREEN[] = "\033[1;32m";
constexpr char YELLOW[] = "\033[1;33m";
constexpr char BLUE[] = "\033[1;34m";
constexpr char RESET[] = "\033[0m";

void clearScreen() {
    // Clear screen command based on platform
#ifdef _WIN32
    std::system("cls");
#else
    std::system("clear");
#endif
}

void pressEnterToContinue() {
    cout << YELLOW << "\n[Press Enter to Continue]" << RESET;
    cin.ignore(); // Wait for Enter key press
}

void printMenu() {
    cout << BLUE << "1. " << RESET << "Send Message\n";
    cout << GREEN << "2. " << RESET << "Follow User\n";
    cout << RED << "3. " << RESET << "Exit\n";
    cout << "Choose an option: ";
}

int main() {
    int userId = 15;
    string str;
    User user(userId);
    Session session = Session();

    int choice;
    while (true) {
        clearScreen();
        printMenu();
        cin >> choice;
        cin.ignore(); // Consume newline character

        switch (choice) {
            case 1: {
                cout << "Write down a message!" << endl;
                getline(cin, str);
                session.sendMessage(str);
                pressEnterToContinue();
                break;
            }
            case 2: {
                string following;
                cout << "Enter the ID of the user you want to follow: ";
                cin >> following;
                cin.ignore(); // Consume newline character
                session.sendFollow(following); // Replace "username" with the actual username
                pressEnterToContinue();
                break;
            }
            case 3: {
                session.sendExit();
                cout << YELLOW << "Exiting the application.\n" << RESET;
                pressEnterToContinue();
                return 0;
            }
            default:
                cout << RED << "Invalid choice. Please try again.\n" << RESET;
                pressEnterToContinue();
        }
    }
}
