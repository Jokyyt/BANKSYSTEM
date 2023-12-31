#include "..\bankfunctions\bank_system.h"
#include "..\bankfunctions\general_utilities.h"
#include "..\bankfunctions\user_management.h"
#include "..\bankfunctions\verification.h"
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <errno.h>
#include <stdbool.h>
#include <ctype.h>
#include "..\Librairies\cJSON\cJSON.h"


// Définir des couleurs pour la console
void SetColor(int ForgC) {
    WORD wColor;
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    if (GetConsoleScreenBufferInfo(hStdOut, &csbi)) {
        wColor = (csbi.wAttributes & 0xF0) + (ForgC & 0x0F);
        SetConsoleTextAttribute(hStdOut, wColor);
    }
}

// Fonction pour afficher le menu principal
void displayMainMenu() {
    SetColor(15); // Blanc
    printf("\n");
    printf("\n******************************************\n");
    printf("*      WELCOME TO BANK ACCOUNT SYSTEM    *\n");
    printf("******************************************\n");
    printf("1.... ALREADY A USER? SIGN IN\n");
    printf("2.... CREATE A BANK ACCOUNT\n");
    printf("3.... EXIT\n\n");
    printf("ENTER YOUR CHOICE: ");
    Sleep(1000);
}

// Fonction pour afficher le menu utilisateur
void displayUserMenu() {
    SetColor(11); // Blanc
    printf("\n");
    printf("\n HOME\n");
    printf("******\n\n");
    printf("1....CHECK INFOS\n");
    printf("2....TRANSFER MONEY\n");
    printf("3....LOG OUT\n");
    printf("4....DELETE ACCOUNT\n");
    printf("5....EXIT\n\n");
    printf("ENTER YOUR CHOICE: ");
    Sleep(1000); 
}


int main() {
    User currentUser;
    int choix;
    int connected = 0; // 0 signifie que l'utilisateur n'est pas connecté

    while (1) {
        if (!connected) {
            displayMainMenu();
            choix = getIntegerInput();
            
            switch (choix) {
                case 1:
                    if (Login(&currentUser) == 0) {
                        connected = 1;
                        Sleep(1000);
                    } else {
                        Sleep(1000);
                        printf("CONNECTION ERROR\n");
                    }
                    break;

                case 2:
                    if (createAccount(&currentUser) == 0) {
                        Sleep(1000);
                        connected = 1;
                    } else {
                        Sleep(1000);
                        printf("Account creation error\n");
                        return 0;
                    }
                    break;

                case 3:
                    printf("Thank you for using our service. Goodbye!\n");
                    return 0; // Quitter de manière propre

                default:
                    printf("Invalid option. Try again.\n");
                    break;
            }
        } else {
            displayUserMenu();
            choix = getIntegerInput();

            switch (choix) {
                case 1:
                    Sleep(500);
                    get_infos(&currentUser);
                    break;

                case 2:
                    transferAmountBetweenUsers();
                    break; 

                case 3: // Option pour la déconnexion
                    printf("Disconnection in progress...\n");
                    Sleep(1500);
                    connected = 0;
                    break;

                case 4: // Option pour supprimer un utilisateur
                    deleteUser(&currentUser);
                    connected = 0; // Déconnecter l'utilisateur après la suppression
                    printf("REDIRECT TO LOGIN MENU...\n");
                    Sleep(1500);
                    break;

                case 5: 
                    printf("Thank you for using our service. Goodbye!\n");
                    return 0; // Quitter de manière propre

                default:
                    printf("Invalid option. Try again.\n");
                    break;
            }
        }
    }
    return 0; // Quitter
}
