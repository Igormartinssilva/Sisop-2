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
    cout << BLUE << ">>-- Welcome to Y --<<" << RESET << std::endl << std::endl; 
    cout << RED << "1. " << RESET << "Send Message\n";
    cout << RED << "2. " << RESET << "Follow User\n";
    cout << RED << "3. " << RESET << "Exit\n";
    cout << BLUE <<"Choose an option: " << RESET;
}

int main(int argc, char **argv) {
    Session session(argv[1]);
    string str;
    string username;

    if (argc < 2)
        cerr << "you must inform IP"<< endl;
    
    clearScreen();
    cout << BLUE << ">>-- Welcome to Y --<<" << RESET << endl;

    cout << "Connecting to server " << RED << argv[1] << RESET << " at port " << RED << PORT << RESET << "..." << endl;
    cout << BLUE << "Insert your username: " << RESET;
    getline(cin, username);

    session.sendLogin("Luis");
    if (!session.isLogged())
        return 0;
    
    pressEnterToContinue();
    std::thread processingThread(&Session::processBuffer, session);
    std::thread receivingThread(&Session::processReceiving, session);


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
                cout << "Enter the username of the user you want to follow: ";
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
    processingThread.join();
    receivingThread.join();
    return 0;
}
