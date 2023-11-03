#include "bankfunctions.h"
#include <stdio.h>   // Inclure <stdio.h> pour FILE, NULL, etc.
#include <stddef.h> // Inclure <stddef.h> pour NULL
#include <string.h>
#include <windows.h>
#include <errno.h>
#include <conio.h>
#include <stdbool.h>
#include "Librairies\cJSON\cJSON.h"

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

    printf("!!!!!CREATE ACCOUNT!!!!!\n");
    cJSON *newUser = cJSON_CreateObject();

    do {

        // Lire les données de l'utilisateur à créer
        char username_input[21];
        char password_input[21];

        do {
            printf("USERNAME : ");
            scanf("%20s", username_input);

            // Vérifier si le mot de passe respecte les règles
            if (!verif_username_len(username_input)) {
                printf(ERROR_USERNAME_REQUIREMENTS);
            }
        } while (!verif_username_len(username_input)); // Continuer de demander le username tant qu'il n'est pas valide
        

        do {
            printf("PASSWORD : ");
            scanf("%20s", password_input);

            // Vérifier si le mot de passe respecte les règles
            if (!verif_password(password_input)) {
                printf(ERROR_PASSWORD_REQUIREMENTS);
            }
        } while (!verif_password(password_input)); // Continuer de demander le mot de passe tant qu'il n'est pas valide

        userExistsResult = checkInfos(user, username_input, password_input);

        // ... (code pour vérifier l'existence de l'utilisateur, comme vous l'avez déjà)

        if (userExistsResult == 0) {
            cJSON_AddStringToObject(newUser, "ID", generateRandomID());
            cJSON_AddStringToObject(newUser, "username", username_input);
            cJSON_AddStringToObject(newUser, "password", password_input);
            cJSON_AddNumberToObject(newUser, "solde", 0.0); // Solde initial à 0
            

            user->ID = strdup(cJSON_GetObjectItem(newUser, "ID")->valuestring);
            user->username = strdup(username_input);
            user->password = strdup(password_input);
            user->solde = 0.0; // Initialiser le solde à 0.0

            cJSON_AddItemToArray(userArray, newUser); // Ajouter le nouvel utilisateur au tableau d'utilisateurs

            // Réécrire le fichier JSON avec le nouvel utilisateur
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
            
            printf("\nFetching account details.....\n");
            Sleep(1000);
            printf("\nAccount created successfully!\n");
            printf("\n================================================================\n\n");
            printf("CURRENT LOGIN...\n");

            return 0; // Ou tout autre code de succès que vous préférez
        } else {
            printf("Username already in use for this account. Please try again");
        }
    } while (userExistsResult != 0);

    return -1; // Ajoutez un code d'erreur si vous sortez de la boucle pour une autre raison
}


// Fonction de connexion de l'utilisateur
int Login(User *user) {
    char input_username[50];
    char input_password[50];
    printf("==== LOG IN ====\n");

    printf("Enter your username: ");
    scanf("%49s", input_username);
    printf("Enter your password: ");
    scanf("%49s", input_password);
    
    int userFoundResult = checkInfos(user, input_username, input_password);
    
    if (userFoundResult == 3) {
        user->username = strdup(input_username); // Assurez-vous d'allouer de la mémoire pour le champ username
        user->password = strdup(input_password); // Assurez-vous d'allouer de la mémoire pour le champ password
        Sleep(1000);
        printf("\n");
        printf("LOGIN SUCCESSFUL....\n\n");
        printf("Press ENTER to continue...");
        getch();  // Attend que l'utilisateur appuie sur Enter
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
            user->ID = strdup(cJSON_GetObjectItem(userObj, "ID")->valuestring);
            user->username = strdup(storedUsername);
            user->password = strdup(storedPassword);
            user->solde = cJSON_GetObjectItem(userObj, "solde")->valuedouble;
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

// Fonction pour charger un utilisateur à partir de son ID
int loadUserByID(User *user, const char *userID) {
    FILE *fichier = fopen(JSON_FILE_PATH, "r");

    if (fichier == NULL) {
        perror(ERROR_OPEN_FILE);
        return 0; // Échec du chargement de l'utilisateur
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
        return 0; // Échec du chargement de l'utilisateur
    }

    cJSON *userArray = cJSON_GetObjectItem(root, "users");
    if (!userArray) {
        cJSON_Delete(root);
        perror(ERROR_GETTING_USER_ARRAY);
        return 0; // Échec du chargement de l'utilisateur
    }

    for (int i = 0; i < cJSON_GetArraySize(userArray); i++) {
        cJSON *userObj = cJSON_GetArrayItem(userArray, i);
        const char *storedID = cJSON_GetObjectItem(userObj, "ID")->valuestring;

        if (strcmp(userID, storedID) == 0) {
            user->ID = strdup(cJSON_GetObjectItem(userObj, "ID")->valuestring);
            user->username = strdup(cJSON_GetObjectItem(userObj, "username")->valuestring);
            user->password = strdup(cJSON_GetObjectItem(userObj, "password")->valuestring);
            user->solde = cJSON_GetObjectItem(userObj, "solde")->valuedouble;
            
            cJSON_Delete(root);
            return 1; // Succès du chargement de l'utilisateur
        }
    }

    cJSON_Delete(root);
    return 0; // Échec du chargement de l'utilisateur, l'ID n'a pas été trouvé
}

// FONCTIONS DE GESTION DU COMPTE UTILISATEUR  

int transferAmountBetweenUsers() {
    User sender, receiver;
    float amount;
    char senderID[9], receiverID[9];
    printf("\n");
    printf("========================\n");
    printf("---- TRANSFER MONEY ----\n");
    printf("========================\n");
    printf("\n");


    printf("FROM (your ID): ");
    scanf("%8s", senderID);

    printf("TO (ID of person): ");
    scanf("%8s", receiverID);

    // Charger les détails de l'expéditeur et du destinataire à partir du fichier JSON en utilisant leurs ID
    if (loadUserByID(&sender, senderID) && loadUserByID(&receiver, receiverID)) {
        printf("ENTER THE AMOUNT TO BE TRANSFERRED: ");
        scanf("%f", &amount);

        // Appel de la fonction de transfert
        return transfer(&sender, &receiver, amount);
    } else {
        printf("One or both users were not found.\n");
        return -1;
    }
}

int transfer(User *sender, User *receiver, float amount) {
    if (sender->solde >= amount) {
        sender->solde -= amount;
        receiver->solde += amount;

        // Mettre à jour les soldes des utilisateurs dans le fichier JSON
        updateSoldeUser(sender);
        updateSoldeUser(receiver);


        printf("--------------------------------------------------------\n");
        printf("--------------------------------------------------------\n");
        printf("Transferring amount, Please wait..\n");
        printf("****************************************\n");
        Sleep(2000);
        printf("AMOUNT SUCCESSFULLY TRANSFERRED....\n");

        printf("\nPress ENTER to return to Home Menu...");
        getch();  // Attend que l'utilisateur appuie sur Enter
        return 0;
    } else {
        printf("Insufficient balance to complete this transfer.\n");
        return -1; // Échec du transfert
    }
}



// Fonction d'affichage des informations de l'utilisateur
void get_infos(User *user) {
    User tempUser;
    if (loadUserByID(&tempUser, user->ID)) {
        printf("--------------------------------\n");
        printf("----------- MY INFOS -----------\n");
        printf("--------------------------------\n");
        printf("MY ID: %s\n", tempUser.ID);
        printf("USERNAME: %s\n", tempUser.username);
        printf("PASSWORD: %s\n", tempUser.password);
        printf("SOLDE: %.2f $\n", tempUser.solde);
        printf("\nPress ENTER to return to Home Menu...");
        getch();  // Attend que l'utilisateur appuie sur Enter
    } else {
        printf("User not found.\n");
    }
}



// Fonction de mise à jour du solde de l'utilisateur dans le fichier JSON
void updateSoldeUser(User *user) {
    cJSON *root = NULL;

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

    root = cJSON_Parse(json_str);
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
        const char *storedID = cJSON_GetObjectItem(userObj, "ID")->valuestring;

        if (strcmp(user->ID, storedID) == 0) {
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
                deleteIDFromFile(user->ID);
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
                printf("YOUR ACCOUNT HAS BEEN DELETED...\n");
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
        printf("ARE YOU SURE YOU WANT TO CONTINUE ? (y/n) --> ");
        scanf("%s", choice);
        
        if (strcmp(choice, "y") == 0 || strcmp(choice, "Y") == 0 || strcmp(choice, "yes") == 0 || strcmp(choice, "Yes") == 0 || strcmp(choice, "YES") == 0) {
            return true;
        } else if (strcmp(choice, "n") == 0 || strcmp(choice, "N") == 0 || strcmp(choice, "no") == 0 || strcmp(choice, "No") == 0 || strcmp(choice, "NO") == 0) {
            return false;
        } else {
            printf("INVALID choice. Please try again.\n");
            tentatives++;
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