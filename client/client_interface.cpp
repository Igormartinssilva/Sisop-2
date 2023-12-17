// main.cpp

#include "header/session.hpp"
#include <cstdlib> // for std::system
#include "../common/header/data.hpp"
#include "../common/header/utils.hpp"
#include <semaphore.h>
using namespace std;

// Variável global para armazenar a sessão
Session* globalSession = nullptr;

void printMenu(std::string username) {
    cout << BLUE << ">>-- Welcome to Y --<<" << RESET << "  connected as @" << username << std::endl << std::endl;
    cout << "Hello, " << BLUE << username << RESET << " (@" << username << ")!" << std::endl << std::endl;
    cout << PURPLE << "1. " << RESET << "Send Message\n";
    cout << PURPLE << "2. " << RESET << "Follow User\n";
    cout << PURPLE << "3. " << RESET << "List Messages\n";
    cout << PURPLE << "4. " << RESET << "Exit\n";
    cout << BLUE <<"Choose an option: " << RESET;
}

void printMenuLoggedOut() {
    cout << BLUE << ">>-- Welcome to Y --<<" << RESET << std::endl << std::endl;
    cout << BLUE << "Insert your username [deixe em branco para sair]: " << RESET;
}

void signalHandler(int signum) {
    std::cout << "Intercepted signal " << signum << std::endl;

    globalSession->sendExit();

    exit(signum);
}

int main(int argc, char **argv) {
    Session session(argv[1]);
    string str;
    string username;
    globalSession = &session;
    int problemLogin;

    // Inicializa os semáforos
    twt::initializeSemaphores();
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    if (argc < 2)
        cerr << "you must inform IP"<< endl;
    
    while (!session.isLogged()){
        clearScreen();
        cout << "Connecting to server " << RED << argv[1] << RESET << " at port " << RED << PORT << RESET << "..." << endl;
        printMenuLoggedOut();
        std::getline(std::cin, username);
        if (username.compare("") == 0){
            cout << "Encerrando programa..." << endl;
            return 0;
        }
        while (!nameConsistency(username)) {
            cout << endl;
            cout << "Nome de usuario deve ter entre 4 e 20 caracteres.\n";
            cout << "Nome de usuario deve conter apenas letras e digitos\n";
            pressEnterToContinue();
            clearScreen();
            printMenuLoggedOut();
            std::getline(std::cin, username);
        }
        problemLogin = session.sendLogin(username);
        if (!session.isLogged() && problemLogin == 1)
            cout << "Nao foi possivel fazer login. Numero maximo de sessoes por conta excedido" << endl;
        else if (!session.isLogged() && problemLogin == 0)
            cout << "Nao foi possivel fazer login. Nao foi possivel se conectar ao servidor" << endl;
        pressEnterToContinue();
    }
    
    

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
