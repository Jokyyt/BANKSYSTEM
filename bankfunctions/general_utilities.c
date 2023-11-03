#include "bank_system.h"
#include "general_utilities.h"
#include "user_management.h"
#include "verification.h"
#include <stdio.h>   // Inclure <stdio.h> pour FILE, NULL, etc.
#include <stddef.h> // Inclure <stddef.h> pour NULL
#include <string.h>
#include <windows.h>
#include <errno.h>
#include <conio.h>
#include <stdbool.h>
#include "..\Librairies\cJSON\cJSON.h"

// Définir des constantes pour les noms de fichiers
#define JSON_FILE_PATH "DATA/users.json"
#define ID_USED_FILE_PATH "DATA/id_used.txt"

// Définir des constantes pour les messages d'erreur
#define ERROR_OPEN_FILE "Error opening file"
#define ERROR_PARSING_JSON "Error parsing JSON"
#define ERROR_GETTING_USER_ARRAY "Error getting user array from JSON"
#define ERROR_WRITING_FILE "Error writing to file\n"
#define ERROR_OPEN_FILE_WRITE "Error opening file for writing"
#define ERROR_PASSWORD_REQUIREMENTS "Password requirements: At least 8 characters, including one uppercase letter, one digit, and one symbol\n"
#define ERROR_USERNAME_REQUIREMENTS "User name requirements: Length must be between 3 and 20 characters\n"





// Fonction de confirmation de choix
bool confirm_choice() {
    char choice[10];
    int tentatives = 0;
    
    while (tentatives < 3) {
        printf("ARE YOU SURE YOU WANT TO CONTINUE ? (y/n) --> ");
        if (scanf("%9s", choice) == 1) { // Limiter la saisie à 9 caractères
            if (strcmp(choice, "y") == 0 || strcmp(choice, "Y") == 0 || strcmp(choice, "yes") == 0 || strcmp(choice, "Yes") == 0 || strcmp(choice, "YES") == 0) {
                return true;
            } else if (strcmp(choice, "n") == 0 || strcmp(choice, "N") == 0 || strcmp(choice, "no") == 0 || strcmp(choice, "No") == 0 || strcmp(choice, "NO") == 0) {
                return false;
            } else {
                printf("INVALID choice. Please try again.\n");
                tentatives++;
            }
        } else {
            printf("ERROR: Wrong type or input too long. Please try again.\n");
            fflush(stdin); // Effacer le buffer d'entrée
        }
    }

    printf("Too many attempts. Return to HOME MENU...\n");
    Sleep(1500);
    return false;
}

// Fonction pour générer un ID aléatoire de longueur 8
char *generateRandomID() {
    static const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char *randomID = (char *)malloc(9 * sizeof(char)); // 8 caractères + 1 pour le caractère nul

    if (randomID) {
        for (int i = 0; i < 8; i++) {
            int index = rand() % (sizeof(charset) - 1);
            randomID[i] = charset[index];
        }
        randomID[8] = '\0';  // Terminer la chaîne
    }

    // Vérifier si l'ID généré est déjà utilisé
    if (checkID(randomID) == 1) {
        free(randomID);
        return generateRandomID(); // Générer un nouvel ID
    } else {
        // ID est unique, ajoutons-le au fichier
        FILE *fichier;
        fichier = fopen(ID_USED_FILE_PATH, "a"); // Mode "a" pour ajouter à la fin du fichier

        if (fichier == NULL) {
            perror(ERROR_WRITING_FILE);
            return NULL;
        }

        // Écrire le nouvel ID dans le fichier
        fprintf(fichier, "%s\n", randomID);

        fclose(fichier); // Fermer le fichier après avoir ajouté l'ID
    }
    return randomID;
}


int checkID(const char *ID) {
    FILE *fichier;
    fichier = fopen(ID_USED_FILE_PATH, "r");

    if (fichier == NULL) {
        perror(ERROR_OPEN_FILE);
        return -1;
    }

    char ligne[100]; // Vous pouvez ajuster la taille en fonction de vos besoins

    while (fgets(ligne, sizeof(ligne), fichier) != NULL) {
        ligne[strlen(ligne) - 1] = '\0'; // Supprimer le saut de ligne
        if (strcmp(ID, ligne) == 0) {
            fclose(fichier);
            return 1; // L'ID est déjà utilisé
        }
    }

    fclose(fichier); // Fermer le fichier après utilisation
    return 0; // L'ID est disponible
}

/*
Cette fonction prend l'ID à supprimer en paramètre et parcourt le fichier used_id.txt.
Si l'ID correspond à celui qui doit être supprimé, il ne le copie pas dans un fichier temporaire.
Ensuite, il supprime le fichier d'origine et renomme le fichier temporaire avec le nom du fichier d'origine, éliminant ainsi l'ID spécifique.
*/

void deleteIDFromFile(const char *id_to_delete) {
    FILE *file = fopen("DATA/id_used.txt", "r");
    FILE *temp_file = fopen("DATA/temp.txt", "w");

    if (file == NULL || temp_file == NULL) {
        printf("Erreur lors de l'ouverture des fichiers.\n");
        exit(1);
    }

    char line[50]; // Vous pouvez ajuster la taille en fonction de vos besoins

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0; // Supprimer le saut de ligne

        if (strcmp(id_to_delete, line) != 0) {
            fprintf(temp_file, "%s\n", line);
        }
    }

    fclose(file);
    fclose(temp_file);

    remove("DATA/id_used.txt");
    rename("DATA/temp.txt", "DATA/id_used.txt");
}

// Fonction pour la saisie sécurisée d'un entier
int getIntegerInput() {
    int input;
    int validInput = 0;

    while (!validInput) {
        if (scanf("%d", &input) == 1) {
            validInput = 1; // Sortir de la boucle si l'entrée est un entier
        } else {
            printf("WRONG TYPE. Please enter an integer: ");
            fflush(stdin); // Effacer le buffer d'entrée
        }
    }

    return input;
}