#include "header.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <iostream>
#include <string.h>
#include <cstdlib>
#include <ctime>


using namespace std;


void menu(usermains user)
{
    int opc = 1;

    while (opc != 0)
    {
        printf("O que voce deseja fazer usuario %s?\n\n", user.userName.c_str());
        cout << "opcao 0 - sair" << endl;
        cout << "opcao 1 - escrever mensagem" << endl;
        cout << "opcao 2 - ler notificacoes" << endl;
        cout << "opcao 3 - ver timestap da sua ultima mensagem" << endl;
        cout << "opcao 4 - ver meu nome e id" << endl;

        cin >> opc;


        if (opc == 0)
        {
            cout << "Saindo do programa... Volte sempre" << endl;
        }
        else if (opc == 1)
        {
            user = sendMessage(user);
        }
        else if (opc == 2)
        {
            cout << "Não implementado\n";
        }
        else if (opc == 3)
        {
            cout << "Data e hora da ultima mensagem enviada: " << user.timestamp << endl;
        }
        else if (opc == 4)
        {
            printf("\nO  nome do usuario e: %s e seu id: %d \n\n", user.userName.c_str(), user.id);
        }
        else
        {
            cout << "Opção inválida. Digite novamente." << endl;
        }
    }
}


int main()
{
    usermains login;

    login = login_user();
    
    cout << "\n\nLogin realizado com sucesso\n";
   
    menu(login);
   


    return 0;
}