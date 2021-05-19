#pragma once
#include <cstdio>
#include <iostream>
#include "App.h"
#include "Enclave_u.h"
#include "App.h"

int read_password(char* target);
int get_option_choice();
int create_vault();
int unlock_vault();
int add_service();
int get_service_names();
int get_service_credentials();
int change_master_password();
int remove_vault();
int remove_item();
void is_password_menu();
void no_password_menu();
void handle_password_error(int error_code);
void vault_password();
void vault_no_password();
void create_vault_menu();
void validate_input(int max, int min);
void clear_user_interface_memory();
