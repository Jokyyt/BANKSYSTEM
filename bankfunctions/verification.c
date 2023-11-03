#include "bank_system.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>


// Fonction de vérification de la longueur du nom d'utilisateur
bool verif_username_len(const char *username) {
    int longueur = strlen(username);
    if (longueur < 3 || longueur > 20) {
        return false;
    }
    return true;

}

// Fonction de vérification du mot de passe
bool verif_password(const char *password) {
    int longueur = strlen(password);
    bool majuscule_presente = false;
    bool chiffre_present = false;
    bool symbole_present = false;
    char symboles[] = "!@#$%^&*(),.?\"':;{}|<>";

    if (longueur < 8 || longueur > 20) {
        return false;
    }

    for (int i = 0; i < longueur; i++) {
        char caractere = password[i];

        if (caractere >= 'A' && caractere <= 'Z') {
            majuscule_presente = true;
        } else if (caractere >= '0' && caractere <= '9') {
            chiffre_present = true;
        } else if (strchr(symboles, caractere) != NULL) {
            symbole_present = true;
        }
    }

    return majuscule_presente && chiffre_present && symbole_present;
}