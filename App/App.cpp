#include <cstdio>
#include <tchar.h>
#include "App.h"
#include "sgx_urts.h"
#include "Enclave_u.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include "UserInterface.h"

using namespace std;

vault_t vault;
sgx_enclave_id_t enclaveId = NULL;
sgx_launch_token_t token = {0};
int updated;

sgx_enclave_id_t get_enclave_id()
{
	return enclaveId;
}

bool initializeEnclave()
{
	if (sgx_create_enclave(ENCLAVE_FILE, SGX_DEBUG_FLAG, &token, &updated,
	                       &enclaveId, nullptr) != SGX_SUCCESS)
	{
		cout << "Fatal error! Creation of enclave wasn't successful." << endl;
		return false;
	}
	return true;
}

bool destroyEnclave()
{
	if (sgx_destroy_enclave(enclaveId) != SGX_SUCCESS)
	{
		cout << "Failed to destroy enclave!" << endl;
		return false;
	}
	return true;
}

void o_save_vault(const unsigned char* sealed_data, size_t sealed_size, int* o_return_value)
{
	FILE* outfile;
	errno_t err;
	err = fopen_s(&outfile, VAULT_FILE, "wb");
	if (err != 0)
	{
		printf("Failed to open File %s", VAULT_FILE);
		*o_return_value = SGX_ERROR_FILE_CANT_OPEN_RECOVERY_FILE;
		return;
	}
	for (auto i = 0; static_cast<size_t>(i) < sealed_size; i++)
	{
		fputc(sealed_data[i], outfile);
	}
	fclose(outfile);
	*o_return_value = SGX_SUCCESS;
}

int o_vault_exists()
{
	ifstream file(VAULT_FILE, ios::in | ios::binary);
	if (file.fail()) { return FAILURE; }
	file.close();
	return SGX_SUCCESS;
}

void o_read_vault(unsigned char** sealed_data, size_t* sealed_size, int* o_return_value)
{
	*sealed_size = readFromFile(sealed_data, o_return_value);
}

void unexpected_error()
{
	system("CLS");
	printf("Unexpected error!\nProgram will be terminated!\n");
}

long readFromFile(unsigned char** sealed_data, int* o_return_value)
{
	FILE* infile;
	errno_t err;
	long file_size = 0;
	err = fopen_s(&infile, VAULT_FILE, "rb");
	if (err == 0)
	{
		fseek(infile, 0L, SEEK_END);
		file_size = ftell(infile);
		rewind(infile);
		*sealed_data = static_cast<unsigned char*>(calloc(file_size, sizeof(unsigned char)));
		unsigned char* tmp = *sealed_data;
		size_t len = fread(tmp, sizeof(unsigned char), file_size, infile);
		fclose(infile);
	}
	else
	{
		printf("Failed to open File %s", VAULT_FILE);
		*o_return_value = SGX_ERROR_FILE_CANT_OPEN_RECOVERY_FILE;
	}
	*o_return_value = SGX_SUCCESS;
	return file_size;
}

void remove_file()
{
	const int result = remove(VAULT_FILE);
	unsigned const int max = 100;
	char buffer[max];
	if (result == 0)
	{
		printf("\nVault successfully removed.\n");
	}
	else
	{
		strerror_s(buffer, max, errno);
		printf("Error: %s\n", buffer);
	}
}

int main()
{
	const int menu_min = 0;
	const int menu_is_password = 6;
	const int menu_no_password = 5;
	int run = 1, option = 1;
	int user_interface_response = 0;
	int status = -999;
	int is_password = 1;
	int return_value;


	if (!initializeEnclave())
	{
		return EXIT_FAILURE;
	}

	if (o_vault_exists() == 1)
	{
		user_interface_response = create_vault();
	}

	if (user_interface_response != 0)
	{
		clear_user_interface_memory();
		e_clear_enclave_memory(enclaveId, &return_value);
		destroyEnclave();
		printf("\nGoodbye!\n");
		return EXIT_FAILURE;
	}
	is_password = unlock_vault();
	if (is_password != 1 && is_password != 0)
	{
		clear_user_interface_memory();
		e_clear_enclave_memory(enclaveId, &return_value);
		destroyEnclave();
		return EXIT_FAILURE;
	}


	while (run != 0)
	{
		if (is_password == FAILURE)
		{
			no_password_menu();
			validate_input(menu_no_password, menu_min);
		}
		else
		{
			is_password_menu();
			validate_input(menu_is_password, menu_min);
		}

		switch (get_option_choice())
		{
		case 0:
			cout << "\nGood bye!" << endl;
			run = 0;
			break;
		case 1:
			status = add_service();
			if (status == ERROR_UNEXPECTED_INPUT)
			{
				printf("\nUnexpected input!\n\n");
				cin.clear();
				cin.ignore(INT_MAX, '\n');
				break;
			}
			if (status == ERROR_FULL_VAULT)
			{
				break;
			}
			if (status != SGX_SUCCESS)
			{
				run = 0;
				unexpected_error();
			}
			break;
		case 2:
			status = get_service_names();
			if (status != SGX_SUCCESS && status != ERROR_EMPTY_VAULT)
			{
				run = 0;
				unexpected_error();
			}
			break;
		case 3:
			status = get_service_credentials();
			if (status != SGX_SUCCESS && status != ERROR_EMPTY_VAULT)
			{
				run = 0;
				unexpected_error();
			}
			break;
		case 4:
			status = remove_item();
			if (status != SGX_SUCCESS && status != ERROR_EMPTY_VAULT)
			{
				run = 0;
				unexpected_error();
			}
			break;
		case 5:
			status = remove_vault();
			if (status == SGX_ERROR_UNEXPECTED)
			{
				run = 0;
				unexpected_error();
			}
			if (status == SGX_SUCCESS)
			{
				run = 0;
			}
			break;
		case 6:
			change_master_password();

			break;
		default:
			system("CLS");
			cout << "Invalid option!" << endl;
		}
	}
	clear_user_interface_memory();
	e_clear_enclave_memory(enclaveId, &return_value);

	if (!destroyEnclave())
	{
		return EXIT_FAILURE;
	}
}
