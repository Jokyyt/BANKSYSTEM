#ifndef GENERAL_UTILITIES_H
#define GENERAL_UTILITIES_H

#include <stdbool.h>
#include "bank_system.h"

bool confirm_choice();
int checkID(const char *ID);
char *generateRandomID();
void deleteIDFromFile(const char *id_to_delete);
int getIntegerInput();

#endif  // UTILITIES_H
