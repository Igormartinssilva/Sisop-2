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

    cout << "digite seu nome de usuario: ";
    cin >> nameUser;

    while ( nameUser.length() < 4 )
    {
        cout << "\nnome de usuario curto demais, digite pelo menos 4 caracteres: ";
        cin >> nameUser;
    }
    
   
    return nameUser;

}






int main()
{
    string nomeLogin;


    printf ("ola, mundo");
    nomeLogin = login_user();

    printf("\nO  nome do usuário é: %s", nomeLogin.c_str() );

    cout << "Login realizado com sucesso\n";

    return 0;
}