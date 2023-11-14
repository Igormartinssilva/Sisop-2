#include "header.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <iostream>
#include <string.h>
#include <cstdlib>
#include <ctime>

using namespace std;

int generateRandomId()
{
    // Configura a semente para a geração de números aleatórios com base no tempo atual
    srand(time(0));

    // Gera um número aleatório de até 6 dígitos
    int randomId = rand() % 1000000;

    return randomId;
}

string getCurrentDateTime()
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

usermains login_user()
{
    usermains user;
    

    cout << "Digite seu nome de usuario: ";
    cin >> user.userName;

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

    cout << "\nDigite sua mensagem: ";
    cin.ignore(); // Limpa o buffer do teclado antes de ler a mensagem
    cin >> user.messg;

    while (user.messg.length() > 128)
    {
        cout << "\n\n Mensagem longa demais, digite menos que 128 caracteres!: ";
        cin >> user.messg;
    }
    user.timestamp = getCurrentDateTime();
    return user;
}




int main()
{
    usermains login;

    login = login_user();

    printf("\nO  nome do usuario e: %s e seu id: %d", login.userName.c_str(), login.id );

    cout << "\n\nLogin realizado com sucesso\n";
    login = sendMessage(login);
    cout << "Data e hora: " << login.timestamp << endl;

    return 0;
}