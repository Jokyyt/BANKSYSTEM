#ifndef USER_MANAGEMENT_H
#define USER_MANAGEMENT_H
#include "bank_system.h"

int createAccount(User *user);
int checkInfos(User *user, const char *username, const char *password);
int Login(User *user);
void get_infos(User *user);
void updateSoldeUser(User *user);
int loadUserByID(User *user, const char *userID);
int transferAmountBetweenUsers();
int transfer(User *sender, User *receiver, float amount);
void deleteUser(User *user);

#endif  // USER_MANAGEMENT_H
