#pragma once
#include "Vault.h"
#include <cstring>
#include <cctype>
#include <cstdlib>

constexpr auto MASTER_PASSWORD_MIN_LENGTH = 10;
constexpr auto SERVICE_PASSWORD_MIN_LENGTH = 8;

int validate_master_password(char* master_password);
int validate_service_password(char* service_password);
int passwords_match(const char* first_password, const char* second_password);

