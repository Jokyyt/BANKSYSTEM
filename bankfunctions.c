#include "bankfunctions.h"
#include <stdio.h>   // Inclure <stdio.h> pour FILE, NULL, etc.
#include <stddef.h> // Inclure <stddef.h> pour NULL
#include <string.h>
#include <windows.h>
#include <errno.h>
#include "Librairies\cJSON\cJSON.h"

// FONCTIONS DE GESTION BANCAIRE

// CREER UN COMPTE
int createAccount(User *user) {
        FILE *fichier = fopen("DATA/users.json", "r+");

    if (fichier == NULL) {
        perror("Error opening file");
        return 1;
    }

    char username_input[50];
    char password_input[50];
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
        perror("Error parsing JSON");
        return -1;
    }

    char *usersKey = "users";
    cJSON *userArray = cJSON_GetObjectItem(root, usersKey);

    if (!userArray) {
        cJSON_Delete(root);
        perror("Error getting user array from JSON");
        return -1;
    }

    printf("NEW USER !!\n");
    cJSON *newUser = cJSON_CreateObject();

    do {
        printf("USERNAME : ");
        scanf("%49s", username_input);
        printf("PASSWORD : ");
        scanf("%49s", password_input);

        userExistsResult = checkInfos(user, username_input, password_input);

        if (userExistsResult == 3) {
            printf("Username and/or password already exist. Please choose different values.\n");
        } else if (userExistsResult == 1) {
            printf("Username already exists. Please choose a different username.\n");
        } else if (userExistsResult == 2) {
            printf("Password already exists. Please choose a different password.\n");
        } else {
            user->username = strdup(username_input);
            user->password = strdup(password_input);
            cJSON_AddStringToObject(newUser, "username", username_input);
            cJSON_AddStringToObject(newUser, "password", password_input);
            cJSON_AddNumberToObject(newUser, "solde", 0.0); // Solde initial à 0

            cJSON_AddItemToArray(cJSON_GetObjectItem(root, "users"), newUser);

            fichier = fopen("DATA/users.json", "w"); // Réouvrir en mode écriture
            if (fichier == NULL) {
                perror("Error opening file for writing");
                cJSON_Delete(root);
                return 2;
            }

            if (fprintf(fichier, "%s", cJSON_Print(root)) < 0) {
                perror("Error writing to file");
                fclose(fichier);
                cJSON_Delete(root);
                return 2;
            }

            fclose(fichier);
        }
    } while (userExistsResult != 0);

    cJSON_Delete(root); // Libérer la mémoire de l'objet cJSON

    printf("Account created successfully!\n");
    /*
    printf("Username: %s\n", cJSON_GetObjectItem(newUser, "username")->valuestring);
    printf("Password: %s\n", cJSON_GetObjectItem(newUser, "password")->valuestring);
    printf("Solde: %f\n", cJSON_GetObjectItem(newUser, "solde")->valuedouble);
    */
    return 0;
}



int checkInfos(User *user, const char *username, const char *password) {
    FILE *fichier = fopen("DATA/users.json", "r");

    if (fichier == NULL) {
        perror("Error opening file");
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
        perror("Error parsing JSON");
        return -1;
    }

    cJSON *userArray = cJSON_GetObjectItem(root, "users");
    if (!userArray) {
        cJSON_Delete(root);
        perror("Error getting user array from JSON");
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


int Login(User *user) {
    char input_username[50];
    char input_password[50];

    printf("Enter your username: ");
    scanf("%s", input_username);
    printf("Enter your password: ");
    scanf("%s", input_password);
    
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

void get_infos(User *user) {
    printf("Username: %s\n", user->username);
    printf("Password: %s\n", user->password);
    printf("Solde: %.2f $\n", user->solde);
}


void addsolde(User *user, double amount) {
    printf("Solde: %.2f $\n", user->solde);
    printf("How much would you like to add? --> ");
    scanf("%lf", &amount);
    user->solde += amount; 
    printf("New solde: %.2f $\n", user->solde);
    mettreAjourDonneesUtilisateur(user);
}

void subtractsolde(User *user, double amount) {
    printf("Solde: %.2f $\n", user->solde - amount);
    printf("How much would you like to subtract? --> ");
    scanf("%lf", &amount);
    user->solde -= amount;
    printf("New solde: %.2f $\n", user->solde);
    mettreAjourDonneesUtilisateur(user);
}

void mettreAjourDonneesUtilisateur(User *user) {
    FILE *fichier = fopen("DATA/users.json", "r+");

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

            fichier = fopen("DATA/users.json", "w");

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
}





















/*

int closeAccount(User *user) {
    FILE *fichier = fopen("DATA/users.json", "r+");

    if (fichier == NULL) {
        perror("Error opening file");
        return 1;
    }

    cJSON *root = NULL;
    cJSON *userArray = NULL;

    fseek(fichier, 0, SEEK_END);
    long fsize = ftell(fichier);
    fseek(fichier, 0, SEEK_SET);

    char *json_str = (char *)malloc(fsize + 1);

    if (json_str == NULL) {
        perror("Memory allocation error");
        fclose(fichier);
        return -1;
    }

    if (fread(json_str, 1, fsize, fichier) != fsize) {
        perror("Error reading file");
        free(json_str);
        fclose(fichier);
        return -1;
    }

    fclose(fichier);
    json_str[fsize] = 0;

    root = cJSON_Parse(json_str);
    free(json_str);

    if (!root) {
        perror("Error parsing JSON");
        return -1;
    }

    userArray = cJSON_GetObjectItem(root, "users");

    if (!userArray) {
        cJSON_Delete(root);
        perror("Error getting user array from JSON");
        return -1;
    }

    int userIndexToDelete = -1;

    for (int i = 0; i < cJSON_GetArraySize(userArray); i++) {
        cJSON *userObj = cJSON_GetArrayItem(userArray, i);
        const char *storedUsername = cJSON_GetObjectItem(userObj, "username")->valuestring;
        const char *storedPassword = cJSON_GetObjectItem(userObj, "password")->valuestring;

        int usernameMatch = strcmp(user->username, storedUsername) == 0;
        int passwordMatch = strcmp(user->password, storedPassword) == 0;

        if (usernameMatch && passwordMatch) {
            userIndexToDelete = i;
            break;
        }
    }

    if (userIndexToDelete >= 0) {
        cJSON_DeleteItemFromArray(userArray, userIndexToDelete);
        fichier = fopen("DATA/users.json", "w");

        if (fichier == NULL) {
            cJSON_Delete(root);
            perror("Error opening file for writing");
            return 2;
        }

        if (fprintf(fichier, "%s", cJSON_Print(root)) < 0) {
            perror("Error writing to file");
            fclose(fichier);
            cJSON_Delete(root);
            return 2;
        }

        fclose(fichier);
    } else {
        cJSON_Delete(root);
        printf("No matching accounts found.\n");
        return 1; // Aucun compte correspondant n'a été trouvé.
    }

    cJSON_Delete(root);
    return 0; // Le compte a été supprimé avec succès.
}
*/