#pragma once
#ifndef APP_H_
#define APP_H_
#include <sgx_eid.h>

constexpr auto VAULT_FILE = "vault.sealed";
constexpr auto ENCLAVE_FILE = "Enclave.signed.dll";
constexpr auto FAILURE = 1;
constexpr auto ERROR_PASSWORD_POLICY_FAILED = -1;
constexpr auto ERROR_PASSWORDS_DO_NOT_MATCH = -2;
constexpr auto ERROR_EMPTY_VAULT = -7;
constexpr auto ERROR_FULL_VAULT = -8;
constexpr auto ERROR_PASSWORD_INPUT_TOO_LONG = -9;
constexpr auto ERROR_PASSWORD_INPUT_UNEXPECTED_CHAR = -10;
constexpr auto ERROR_UNEXPECTED_INPUT = -11;
constexpr auto MASTER_PASSWORD_MIN_LENGTH = 10;
constexpr auto SERVICE_PASSWORD_MIN_LENGTH = 8;

sgx_enclave_id_t get_enclave_id();
int o_vault_exists();
bool initializeEnclave();
bool destroyEnclave();
long readFromFile(unsigned char** sealed_data, int* o_return_value);
void remove_file();
void unexpected_error();
#endif
