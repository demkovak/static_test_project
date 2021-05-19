#pragma once
#include "Vault.h"
#include "Sealing.h"
#include "PasswordHandling.h"
#include "Enclave_t.h"
#include "sgx_tseal.h"
#include <tlibc/stdlib.h>
#include <sgx_error.h>
#include <cstring>
#include <cstdio>
#include "mbusafecrt.h"

constexpr auto FAILURE = 1;
constexpr auto ERROR_PASSWORD_POLICY_FAILED = -1;
constexpr auto ERROR_PASSWORDS_DO_NOT_MATCH = -2;
constexpr auto ERROR_FAIL_SEAL = -4;
constexpr auto ERROR_FAIL_UNSEAL = -5;
constexpr auto ERROR_FAIL_SAVE_VAULT = -6;

void e_create_vault(char* master_password, const int* is_password, int* return_value);
void e_unlock_vault(char* master_password, int* return_value);
void e_is_password(int* is_password, int* return_value);
void e_verify_master_password(char* master_password, int* return_value);
void e_add_service(char* service_name, char* username, char* password, int* return_value);
void e_get_services_names(char* services_names, size_t len);
void e_get_credentials(int* index, item_t* item, size_t item_size, int* return_value);
void e_remove_item(int* index, int* return_value);
void e_change_master_password(char* master_password_new, int* return_value);
void e_get_count_items(size_t* count_items, int* return_value);
void e_clear_enclave_memory(int* return_value);

sgx_status_t data_sealing();
sgx_status_t data_unsealing();
sgx_status_t read_vault();
sgx_status_t save_vault();

int verify_master_password(char* master_password);
void clear_unsealed_data();
