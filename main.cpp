#include "header.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <iostream>
#include <string.h>


using namespace std;


string login_user()
{
    string nameUser;

    cout << "Digite seu nome de usuario: ";
    cin >> nameUser;

    while ( nameUser.length() < 4 || nameUser.length() > 20)
    {
        cout << "\nNome de usuario deve ter entre 4 e 20 caracteres.Digite novamete: ";
        cin >> nameUser;
    } 
   
    return nameUser;
}






int main()
{
    string nomeLogin;

    nomeLogin = login_user();

    printf("\nO  nome do usuario é: %s", nomeLogin.c_str() );

    cout << "\n\nLogin realizado com sucesso\n";

    return 0;
}