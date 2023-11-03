#ifndef VERIFICATION_H
#define VERIFICATION_H

#include "bank_system.h"
#include <stdbool.h>


bool verif_username_len(const char *username);
bool verif_password(const char *password);

#endif  // VERIFICATION_H
