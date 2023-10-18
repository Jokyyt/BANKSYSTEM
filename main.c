
#include "bankfunctions.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <errno.h>
#include "Librairies\cJSON\cJSON.h"

int main() {
    User currentUser;
    int choix;
    int connected = 0; // 0 signifie que l'utilisateur n'est pas connecté

    while (1) {
        if (!connected) {
            Sleep(1000);
            printf("1. Log in\n2. Create an account\n3. Exit\nChoose an option: ");
            scanf("%d", &choix);

            switch (choix) {
                case 1:
                    if (Login(&currentUser) == 0) {;
                        connected = 1;
                        printf("You are now logged in!\n");
                        Sleep(1000);
                        break;
                    }
                    else {
                        Sleep(1000);
                        printf("Connection error\n");
                        break;
                    }
                case 2:
                    if (createAccount(&currentUser) == 0) {;
                        Sleep(1000);
                        break;
                    } else {
                        Sleep(1000);
                        printf("Account creation error\n");
                        return 0;
                    }
                case 3:
                    printf("Thank you for using our service. Goodbye!\n");
                    return 0; // Quitter de manière propre
                default:
                    printf("Invalid option. Try again.\n");
                    break;
            }
        } else {
            Sleep(1000);
            printf("\n1. Consult Account\n2. Add\n3. Substract\n4. Remove Account\n4. Log out\n5. Exit\nChoose an option: ");
            scanf("%d", &choix);

            switch (choix) {
                case 1:
                    get_infos(&currentUser);
                    Sleep(1000);
                    break;
                case 2:
                    addsolde(&currentUser, 0);
                    Sleep(1000);
                    break;
                case 3:
                    subtractsolde(&currentUser, 0);
                    Sleep(1000);
                    break;
                case 4:
                    Sleep(1000);
                    break;
                case 5:
                    printf("Disconnection in progress...\n");
                    Sleep(1500);
                    connected = 0;
                    break;
                case 6: 
                    printf("Thank you for using our service. Goodbye!\n");
                    return 0; // Quitter de manière propre
                default:
                    printf("Invalid option. Try again.\n");
                    break;
            }
        }
    }

    return 0;
}
