#include "bankfunctions.h"
#include <stdio.h>   // Inclure <stdio.h> pour FILE, NULL, etc.
#include <stddef.h> // Inclure <stddef.h> pour NULL
#include <string.h>
#include <windows.h>
#include <errno.h>
#include <stdbool.h>
#include "Librairies\cJSON\cJSON.h"

// Définir des constantes pour les noms de fichiers
#define JSON_FILE_PATH "DATA/users.json"

// Définir des constantes pour les messages d'erreur
#define ERROR_OPEN_FILE "Error opening file"
#define ERROR_PARSING_JSON "Error parsing JSON"
#define ERROR_GETTING_USER_ARRAY "Error getting user array from JSON"
#define ERROR_WRITING_FILE "Error writing to file"
#define ERROR_OPEN_FILE_WRITE "Error opening file for writing"
#define ERROR_PASSWORD_REQUIREMENTS "Password requirements: At least 8 characters, including one uppercase letter, one digit, and one symbol\n"
#define ERROR_USERNAME_REQUIREMENTS "User name requirements: Length must be between 3 and 20 characters\n"



// FONCTIONS DE VERIFICATIONS


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



// FONCTIONS DE GESTION DES UTILISATEURS 


int createAccount(User *user) {
    FILE *fichier = fopen(JSON_FILE_PATH, "r+");

    if (fichier == NULL) {
        perror(ERROR_OPEN_FILE);
        return 1;
    }

    char username_input[21];
    char password_input[21];
    int userExistsResult = 0; // Initialisation du résultat de vérification

    fseek(fichier, 0, SEEK_END);
    long fsize = ftell(fichier);
    fseek(fichier, 0, SEEK_SET);

    char *json_str = (char *)malloc(fsize + 1);
    fread(json_str, 1, fsize, fichier);
    fclose(fichier);
    json_str[fsize] = 0;

    cJSON *root = cJSON_Parse(json_str);
    free(json_str);

    if (!root) {
        cJSON_Delete(root);
        perror(ERROR_PARSING_JSON);
        return -1;
    }

    char *usersKey = "users";
    cJSON *userArray = cJSON_GetObjectItem(root, usersKey);

    if (!userArray) {
        cJSON_Delete(root);
        perror(ERROR_GETTING_USER_ARRAY);
        return -1;
    }

    printf("NEW USER !!\n");
    cJSON *newUser = cJSON_CreateObject();

    do {

        do {
            printf("USERNAME : ");
            scanf("%s", username_input);

            // Vérifier si le mot de passe respecte les règles
            if (!verif_username_len(username_input)) {
                printf(ERROR_USERNAME_REQUIREMENTS);
            }
        } while (!verif_username_len(username_input)); // Continuer de demander le username tant qu'il n'est pas valide
        

        do {
            printf("PASSWORD : ");
            scanf("%49s", password_input);

            // Vérifier si le mot de passe respecte les règles
            if (!verif_password(password_input)) {
                printf(ERROR_PASSWORD_REQUIREMENTS);
            }
        } while (!verif_password(password_input)); // Continuer de demander le mot de passe tant qu'il n'est pas valide

        userExistsResult = checkInfos(user, username_input, password_input);

        // ... (code pour vérifier l'existence de l'utilisateur, comme vous l'avez déjà)

        if (userExistsResult == 0) {
            user->username = strdup(username_input);
            user->password = strdup(password_input);
            cJSON_AddStringToObject(newUser, "username", username_input);
            cJSON_AddStringToObject(newUser, "password", password_input);
            cJSON_AddNumberToObject(newUser, "solde", 0.0); // Solde initial à 0

            cJSON_AddItemToArray(cJSON_GetObjectItem(root, "users"), newUser);

            fichier = fopen(JSON_FILE_PATH, "w"); // Réouvrir en mode écriture
            if (fichier == NULL) {
                perror(ERROR_OPEN_FILE_WRITE);
                cJSON_Delete(root);
                return 2;
            }

            if (fprintf(fichier, "%s", cJSON_Print(root)) < 0) {
                perror(ERROR_WRITING_FILE);
                fclose(fichier);
                cJSON_Delete(root);
                return 2;
            }

            fclose(fichier);

            cJSON_Delete(root); // Libérer la mémoire de l'objet cJSON

            printf("Account created successfully!\n");
            return 0; // Ou tout autre code de succès que vous préférez
        }
    } while (userExistsResult != 0);

    return -1; // Ajoutez un code d'erreur si vous sortez de la boucle pour une autre raison
}


// Fonction de connexion de l'utilisateur
int Login(User *user) {
    char input_username[50];
    char input_password[50];

    printf("Enter your username: ");
    scanf("%49s", input_username);
    printf("Enter your password: ");
    scanf("%49s", input_password);
    
    int userFoundResult = checkInfos(user, input_username, input_password);
    
    if (userFoundResult == 3) {
        user->username = strdup(input_username); // Assurez-vous d'allouer de la mémoire pour le champ username
        user->password = strdup(input_password); // Assurez-vous d'allouer de la mémoire pour le champ password
        return 0;
    } else if (userFoundResult == 1) {
        printf("Le username existe, mais le password n'existe pas.");
        return 1;
        
    } else if (userFoundResult == 2) {
        printf("Le password existe, mais le username n'existe pas.");
        return 2;
    } else {
        return 3;
    }
    
}


// Fonction de vérification des informations de l'utilisateur
int checkInfos(User *user, const char *username, const char *password) {
    FILE *fichier = fopen(JSON_FILE_PATH, "r");

    if (fichier == NULL) {
        perror(ERROR_OPEN_FILE);
        return -1;
    }

    char *json_str = NULL;
    fseek(fichier, 0, SEEK_END);
    long fsize = ftell(fichier);
    fseek(fichier, 0, SEEK_SET);

    json_str = (char *)malloc(fsize + 1);
    fread(json_str, 1, fsize, fichier);
    fclose(fichier);
    json_str[fsize] = 0;

    cJSON *root = cJSON_Parse(json_str);
    free(json_str);

    if (!root) {
        cJSON_Delete(root);
        perror(ERROR_PARSING_JSON);
        return -1;
    }

    cJSON *userArray = cJSON_GetObjectItem(root, "users");
    if (!userArray) {
        cJSON_Delete(root);
        perror(ERROR_GETTING_USER_ARRAY);
        return -1;
    }

    for (int i = 0; i < cJSON_GetArraySize(userArray); i++) {
        cJSON *userObj = cJSON_GetArrayItem(userArray, i);
        const char *storedUsername = cJSON_GetObjectItem(userObj, "username")->valuestring;
        const char *storedPassword = cJSON_GetObjectItem(userObj, "password")->valuestring;

        int usernameMatch = strncmp(username, storedUsername, strlen(username)) == 0;
        int passwordMatch = strncmp(password, storedPassword, strlen(password)) == 0;

        if (usernameMatch && passwordMatch) {
            user->username = strdup(storedUsername); // Allouer et copier le nom d'utilisateur
            cJSON_Delete(root);
            return 3; // Le username et le password existent.
        } else if (usernameMatch) {
            cJSON_Delete(root);
            return 1; // Le username existe, mais le password n'existe pas.
        } else if (passwordMatch) {
            cJSON_Delete(root);
            return 2; // Le password existe, mais le username n'existe pas.
        }
    }

    cJSON_Delete(root);
    return 0; // Ni le username ni le password n'existent pas dans la base de données.
}



// FONCTIONS DE GESTION DU COMPTE UTILISATEUR  


// Fonction d'affichage des informations de l'utilisateur
void get_infos(User *user) {
    printf("Username: %s\n", user->username);
    printf("Password: %s\n", user->password);
    printf("Solde: %.2f $\n", user->solde);
}


// Fonction d'ajout de solde à un compte
void addsolde(User *user, double amount) {
    printf("Solde: %.2f $\n", user->solde);
    printf("How much would you like to add? --> ");
    scanf("%lf", &amount);
    user->solde += amount; 
    printf("New solde: %.2f $\n", user->solde);
    updateSoldeUser(user);
}


// Fonction de soustraction de solde d'un compte
void subtractsolde(User *user, double amount) {
    printf("Solde: %.2f $\n", user->solde);
    printf("How much would you like to subtract? --> ");
    scanf("%lf", &amount);
    user->solde -= amount;
    printf("New solde: %.2f $\n", user->solde);
    updateSoldeUser(user);
}


// Fonction de mise à jour du solde de l'utilisateur dans le fichier JSON
void updateSoldeUser(User *user) {
    FILE *fichier = fopen(JSON_FILE_PATH, "r+");

    if (fichier == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        return;
    }

    fseek(fichier, 0, SEEK_END);
    long fsize = ftell(fichier);
    fseek(fichier, 0, SEEK_SET);

    char *json_str = (char *)malloc(fsize + 1);
    fread(json_str, 1, fsize, fichier);
    fclose(fichier);
    json_str[fsize] = 0;

    cJSON *root = cJSON_Parse(json_str);
    free(json_str);

    if (!root) {
        cJSON_Delete(root);
        perror("Erreur d'analyse JSON");
        return;
    }

    cJSON *userArray = cJSON_GetObjectItem(root, "users");
    if (!userArray) {
        cJSON_Delete(root);
        perror("Erreur lors de la récupération du tableau d'utilisateurs depuis le JSON");
        return;
    }

    for (int i = 0; i < cJSON_GetArraySize(userArray); i++) {
        cJSON *userObj = cJSON_GetArrayItem(userArray, i);
        const char *storedUsername = cJSON_GetObjectItem(userObj, "username")->valuestring;

        if (strcmp(user->username, storedUsername) == 0) {
            cJSON_ReplaceItemInObject(userObj, "solde", cJSON_CreateNumber(user->solde));

            fichier = fopen(JSON_FILE_PATH, "w");

            if (fichier == NULL) {
                cJSON_Delete(root);
                perror("Erreur lors de l'ouverture du fichier en écriture");
                return;
            }

            if (fprintf(fichier, "%s", cJSON_Print(root)) < 0) {
                perror("Erreur d'écriture dans le fichier");
                fclose(fichier);
                cJSON_Delete(root);
                return;
            }

            fclose(fichier);
            cJSON_Delete(root);
            return;
        }
    }
    
    cJSON_Delete(root);
    perror("Utilisateur non trouvé dans le fichier JSON. Mise à jour du solde échouée.");
}


// Fonction de suppression d'utilisateur
void deleteUser(User *user) {
    FILE *fichier = fopen(JSON_FILE_PATH, "r+");

    if (fichier == NULL) {
        perror(ERROR_OPEN_FILE);
        return;
    }

    cJSON *root;
    long fsize;
    char *json_str = NULL;

    fseek(fichier, 0, SEEK_END);
    fsize = ftell(fichier);
    fseek(fichier, 0, SEEK_SET);

    json_str = (char *)malloc(fsize + 1);
    fread(json_str, 1, fsize, fichier);
    fclose(fichier);
    json_str[fsize] = 0;

    root = cJSON_Parse(json_str);
    free(json_str);

    if (!root) {
        cJSON_Delete(root);
        perror(ERROR_PARSING_JSON);
        return;
    }

    cJSON *userArray = cJSON_GetObjectItem(root, "users");

    if (!userArray) {
        cJSON_Delete(root);
        perror(ERROR_GETTING_USER_ARRAY);
        return;
    }

    for (int i = 0; i < cJSON_GetArraySize(userArray); i++) {
        cJSON *userObj = cJSON_GetArrayItem(userArray, i);
        const char *storedUsername = cJSON_GetObjectItem(userObj, "username")->valuestring;

        if (strcmp(user->username, storedUsername) == 0) {
            if (confirm_choice()) {
                cJSON_DeleteItemFromArray(userArray, i);

                fichier = fopen(JSON_FILE_PATH, "w");

                if (fichier == NULL) {
                    cJSON_Delete(root);
                    perror(ERROR_OPEN_FILE_WRITE);
                    return;
                }

                if (fprintf(fichier, "%s", cJSON_Print(root)) < 0) {
                    perror(ERROR_WRITING_FILE);
                    fclose(fichier);
                    cJSON_Delete(root);
                    return;
                }

                fclose(fichier);
                cJSON_Delete(root);
                printf("User deleted successfully!\n");
                return;
            }
        }
    }

    printf("User not found in the JSON file. Deletion failed.\n");
}



// FONCTION D'UTILITE GENERALE

// Fonction de confirmation de choix
bool confirm_choice() {
    char choice[10];
    int tentatives = 0;
    
    while (tentatives < 3) {
        printf("Are you sure you want to continue? (y/n) --> ");
        scanf("%s", choice);
        
        if (strcmp(choice, "y") == 0 || strcmp(choice, "Y") == 0 || strcmp(choice, "yes") == 0 || strcmp(choice, "Yes") == 0 || strcmp(choice, "YES") == 0) {
            return true;
        } else if (strcmp(choice, "n") == 0 || strcmp(choice, "N") == 0 || strcmp(choice, "no") == 0 || strcmp(choice, "No") == 0 || strcmp(choice, "NO") == 0) {
            return false;
        } else {
            printf("Invalid choice. Please try again.\n");
            tentatives++;
        }
    }

    printf("Too many attempts. Return to menu.\n");
    return false;

}