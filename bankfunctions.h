#ifndef BANKFUNCTIONS_H
#define BANKFUNCTIONS_H

#include <stdbool.h>

typedef struct User {
    char *username;
    char *password;
    double solde;
} User;

// Déclarez vos fonctions
bool confirm_choice();
int createAccount(User *user);
int checkInfos(User *user, const char *username, const char *password);
int Login(User *user);
void get_infos(User *user);
void updateDataUser(User *user); 
void addsolde(User *user, double amount);
void subtractsolde(User *user, double amount);
void deleteUser(User *user);

#endif
