// main.cpp

#include "header/session.hpp"
#include <cstdlib> // for std::system
#include "../common/header/data.hpp"
#include "../common/header/utils.hpp"
#include <semaphore.h>
using namespace std;
// ANSI color codes
constexpr char RED[] = "\033[1;31m";
constexpr char GREEN[] = "\033[1;32m";
constexpr char YELLOW[] = "\033[1;33m";
constexpr char BLUE[] = "\033[1;34m";
constexpr char PURPLE[] = "\033[1;35m";
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
    std::cin.ignore(); // Wait for Enter key press
}

void printMenu(std::string username) {
    cout << BLUE << ">>-- Welcome to Y --<<" << RESET << std::endl << std::endl;
    cout << "Hello, " << BLUE << username << RESET << " (@" << username << ")!" << std::endl << std::endl;
    cout << PURPLE << "1. " << RESET << "Send Message\n";
    cout << PURPLE << "2. " << RESET << "Follow User\n";
    cout << PURPLE << "3. " << RESET << "List Messages\n";
    cout << PURPLE << "4. " << RESET << "Exit\n";
    cout << BLUE <<"Choose an option: " << RESET;
}

void printMenuLoggedOut() {
    cout << BLUE << ">>-- Welcome to Y --<<" << RESET << std::endl << std::endl;
    cout << BLUE << "Insert your username: " << RESET;
}

int main(int argc, char **argv) {
    Session session(argv[1]);
    string str;
    string username;

    // Inicializa os semáforos
    twt::initializeSemaphores();

    if (argc < 2)
        cerr << "you must inform IP"<< endl;
    
    clearScreen();
    cout << "Connecting to server " << RED << argv[1] << RESET << " at port " << RED << PORT << RESET << "..." << endl;
    printMenuLoggedOut();
    std::getline(std::cin, username);
    while (!nameConsistency(username)) {
        cout << endl;
        cout << "Nome de usuario deve ter entre 4 e 20 caracteres.\n";
        cout << "Nome de usuario deve conter apenas letras e digitos\n";
        pressEnterToContinue();
        clearScreen();
        printMenuLoggedOut();
        std::getline(std::cin, username);
    }
    session.sendLogin(username);
    if (!session.isLogged())
        return 0;
    
    pressEnterToContinue();

    int choice;
    bool running = true;
    while (running) {
        clearScreen();
        printMenu(username);
        std::cin >> choice;
        std::cin.ignore(); // Consume newline character

        switch (choice) {
            case 1: {
                cout << "Write down a message!" << endl;
                std::getline(std::cin, str);
                session.sendMessage(str);
                pressEnterToContinue();
                break;
            }
            case 2: {
                string following;
                cout << "Enter the username of the user you want to follow: ";
                std::cin >> following;
                std::cin.ignore(); // Consume newline character
                session.sendFollow(following); // Replace "username" with the actual username
                pressEnterToContinue();
                break;
            }
            case 3: {
                session.printYourMessages();
                pressEnterToContinue();
                break;
            }
            case 4: {
                session.sendExit();
                cout << YELLOW << "Exiting the application.\n" << RESET;
                running = false;
                pressEnterToContinue();
                break;
            }
            default:
                cout << RED << "Invalid choice. Please try again.\n" << RESET;
                std::cin.ignore();
                pressEnterToContinue();
        }
    }

    twt::destroySemaphores();

    return 0;
}
