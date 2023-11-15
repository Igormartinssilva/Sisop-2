#include "header.h"
#include <iostream>
#include <string>
#include <random>
#include <ctime>

using namespace std;

int generateRandomId()
{
    // Gera um número aleatório de até 6 dígitos usando <random>
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dis(0, 999999);
    return dis(gen);
}

string getCurrentDateTime()
{
    try
    {
        // Obtém o tempo atual
        time_t now = time(0);

        // Converte o tempo para uma estrutura de tempo local
        tm *ltm = localtime(&now);

        // Formata a data e hora como uma string
        char buffer[80];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", ltm);

        return buffer;
    }
    catch (const exception &e)
    {
        cerr << "Erro ao obter a data e hora: " << e.what() << endl;
        return "Erro na obtenção da data e hora";
    }
}

usermains login_user()
{
    usermains user;
    

    cout << "Digite seu nome de usuario: ";
    getline(cin, user.userName);
    while ( user.userName.length() < 4 || user.userName.length() > 20)
    {
        cout << "\nNome de usuario deve ter entre 4 e 20 caracteres. Digite novamete: ";
        cin >> user.userName;
    } 
   
    user.id = generateRandomId();
    return user;
}

usermains sendMessage(usermains user)
{
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Limpa o buffer antes de getline

    cout << "\nDigite sua mensagem: ";
    getline(cin, user.messg);

    while (user.messg.length() > 128)
    {
        cout << "\nMensagem longa demais, digite menos que 128 caracteres!: ";
        getline(cin, user.messg);
    }

    user.timestamp = getCurrentDateTime();
    return user;
}