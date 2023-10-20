#ifndef BANKFUNCTIONS_H
#define BANKFUNCTIONS_H

#include <stdbool.h>
#include "Librairies\cJSON\cJSON.h"

typedef struct User {
    char *ID;
    char *username;
    char *password;
    double solde;
} User;

// User constructor
int checkID(char *ID);
char *generateRandomID();

// Fonctions de vérification
bool verif_username_len(const char *username);
bool verif_password(const char *password);

// Fonctions de gestion du compte utilisateur
int createAccount(User *user);
int checkInfos(User *user, const char *username, const char *password);
int Login(User *user);
void get_infos(User *user);
void updateSoldeUser(User *user);

// Fonctions de gestion du solde
int getUserByID(User *user, const char *userID);
int transferMoney(User *sender, const char *receiverID);

// Fonctions d'administration du compte
void deleteUser(User *user);
bool confirm_choice();

#endif
