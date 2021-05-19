#include "UserInterface.h"
#include <conio.h>

using namespace std;

char* master_password = nullptr;
char user_input[MAX_ITEM_SIZE];
int init = 0;
int* is_password = &init;
int return_value;
int vault_created = 1;
int option_choice;
char input[MAX_ITEM_SIZE];
char input_username[MAX_ITEM_SIZE], input_password[MAX_ITEM_SIZE], input_title[MAX_ITEM_SIZE];
sgx_status_t status;


int read_password(char* target)
{
	char c;
	int counter = 0;
	char* password;
	password = new char[MAX_ITEM_SIZE];
	while (true)
	{
		c = static_cast<char>(_getch());
		if (static_cast<int>(c) == 13)
		{
			password[counter] = '\0';
			break;
		}
		if (counter >= MAX_ITEM_SIZE - 1)
		{
			delete(password);
			return ERROR_PASSWORD_INPUT_TOO_LONG;
		}
		if (isspace(c) || (c == '\b'))
		{
			delete[]password;
			return ERROR_PASSWORD_INPUT_UNEXPECTED_CHAR;
		}
		password[counter] = c;
		counter++;
		cout << "*";
	}
	strcpy_s(target, MAX_ITEM_SIZE, password);
	delete[]password;
	return SGX_SUCCESS;
}

void handle_password_error(int error_code)
{
	system("CLS");
	switch (error_code)
	{
	case ERROR_PASSWORD_INPUT_TOO_LONG:
		printf("Password is too long!\n\n");
		break;
	case ERROR_PASSWORD_INPUT_UNEXPECTED_CHAR:
		printf("Unexpected character in password!\n\n");
		break;
	default:
		printf("Error during password entering!\n\n");
		break;
	}
}

int create_vault()
{
	const int menu_min = 0;
	const int menu_max = 2;
	cout << "Welcome to the best password manager.\n\n";
	cout << "Vault doesn't exist, select if you want vault protected by a master password or not.\n" << endl;
	create_vault_menu();
	validate_input(menu_max, menu_min);
	if (option_choice == 1)
	{
		vault_password();
	}

	if (option_choice == 2)
	{
		vault_no_password();
	}

	return vault_created;
}

void vault_password()
{
	*is_password = 1;
	system("CLS");
	printf("Creating vault protected by a master password.\n\n");
	printf(
		"Master password length must be at least %d.\nPassword must contain at least one lowercase, uppercase and number.\n\n",
		MASTER_PASSWORD_MIN_LENGTH);

	master_password = static_cast<char*>(calloc(MAX_ITEM_SIZE, sizeof(char)));;
	bool success = false;
	while (!success)
	{
		printf("Type password: ");
		int password_result = read_password(master_password);
		if (password_result != SGX_SUCCESS)
		{
			handle_password_error(password_result);
			continue;
		}
		e_create_vault(get_enclave_id(), master_password, is_password, &return_value);
		if (return_value == SGX_SUCCESS)
		{
			system("CLS");
			success = true;
		}
		else
		{
			system("CLS");
			printf("Failed to create vault!\n");
			printf("Password does not contain at least one uppercase, lowercase or number!\n\n");
		}
	}
	system("CLS");
	printf("Vault has been successfully created!\n\n");
	free(master_password);
	master_password = nullptr;
	vault_created = SGX_SUCCESS;
}

void vault_no_password()
{
	*is_password = 0;
	system("CLS");
	printf("Creating vault  without protection by a master password.\n");
	master_password = static_cast<char*>(calloc(MAX_ITEM_SIZE, sizeof(char)));
	e_create_vault(get_enclave_id(), master_password, is_password, &return_value);
	if (return_value != 0)
	{
		printf("Failed to create vault!\n");
		free(master_password);
		master_password = nullptr;
		vault_created = SGX_SUCCESS;
	}
	printf("Vault has been successfully created!\n\n");
	free(master_password);
	master_password = nullptr;
	vault_created = SGX_SUCCESS;
}

void no_password_menu()
{
	printf("Please, choose one option.\n");
	printf(
		"\n1 - Add new item \n2 - Show all stored services \n3 - Show credentials for a service \n4 - Remove item\n5 - Remove vault and quit\n0 - Quit\n\n");
	printf("Your option: ");
}

void is_password_menu()
{
	printf("Please, choose one option.\n");
	printf(
		"\n1 - Add new item \n2 - Show all stored services \n3 - Show credentials for a service \n4 - Remove item\n5 - Remove vault and quit \n6 - Change master password  \n0 - Quit\n\n");
	printf("Your option: ");
}

void create_vault_menu()
{
	printf("--------------------------------------------------\n");
	printf("1. Vault protected by a master password.\n");
	printf("2. Vault without protection by a master password.\n\n");
	printf("0. Exit password manager.\n");
	printf("--------------------------------------------------\n");
	printf("Your option: ");
}

void validate_input(const int max, const int min)
{
	while (!(cin >> option_choice) || option_choice < min || option_choice > max)
	{
		cout << "Bad input - try again: ";
		cin.clear();
		cin.ignore(INT_MAX, '\n');
	}
}

int unlock_vault()
{
	e_is_password(get_enclave_id(), is_password, &return_value);
	if (*is_password == FAILURE)
	{
		return FAILURE;
	}

	if (return_value != SGX_SUCCESS)
	{
		return return_value;
	}
	bool success = false;
	master_password = static_cast<char*>(calloc(MAX_ITEM_SIZE, sizeof(char)));
	while (!success)
	{
		printf("Verify your master password to continue!\n\n");
		printf("Type password: ");
		int password_result = read_password(master_password);
		if (password_result != SGX_SUCCESS)
		{
			handle_password_error(password_result);
			continue;
		}
		e_unlock_vault(get_enclave_id(), master_password, &return_value);
		if (return_value == SGX_SUCCESS)
		{
			system("CLS");
			success = true;
		}
		else
		{
			system("CLS");
			printf("Invalid master password!\n\n");
		}
	}
	free(master_password);
	master_password = nullptr;
	return SGX_SUCCESS;
}

int add_service()
{
	size_t init_t = 0;
	size_t* count_items = &init_t;

	e_get_count_items(get_enclave_id(), count_items, &return_value);
	if (*count_items == MAX_ITEMS)
	{
		printf("\nVault is full!\n\n\n");
		return ERROR_FULL_VAULT;
	}
	char *title, *username, *password;
	title = new char[MAX_ITEM_SIZE];
	username = new char[MAX_ITEM_SIZE];
	password = new char[MAX_ITEM_SIZE];

	cin.clear();
	cin.ignore(INT_MAX, '\n');
	system("CLS");
	printf("Name of item: ");
	scanf_s("%s", &input_title, MAX_ITEM_SIZE);
	if (input_title[0] == '\0')
	{
		return ERROR_UNEXPECTED_INPUT;
	}
	title = &input_title[0];
	printf("\n");

	printf("Username: ");
	scanf_s("%s", &input_username, MAX_ITEM_SIZE);
	if (input_username[0] == '\0')
	{
		return ERROR_UNEXPECTED_INPUT;
	}
	username = &input_username[0];
	printf("\n");

	printf("Password: ");
	scanf_s("%s", &input_password, MAX_ITEM_SIZE);
	if (input_password[0] == '\0')
	{
		return ERROR_UNEXPECTED_INPUT;
	}
	password = &input_password[0];

	e_add_service(get_enclave_id(), title, username, password, &return_value);
	system("CLS");

	while (return_value != SGX_SUCCESS)
	{
		if (return_value == ERROR_PASSWORD_POLICY_FAILED)
		{
			printf(
				"Password policy failed!\n\nPassword should should have length at least %d characters and not more than %d characters.\nMust contain at least one number or letter.\n\n",
				SERVICE_PASSWORD_MIN_LENGTH, MAX_ITEM_SIZE);
			printf("Password: ");
			scanf_s("%s", &input_password, MAX_ITEM_SIZE);
			if (input_password[0] == '\0')
			{
				return ERROR_UNEXPECTED_INPUT;
			}
			password = &input_password[0];
			e_add_service(get_enclave_id(), title, username, password, &return_value);
			system("CLS");
		}
		else
		{
			title = nullptr;
			username = nullptr;
			password = nullptr;
			return return_value;
		}
	}
	printf("New item has been successfully saved!\n\n\n");

	title = nullptr;
	username = nullptr;
	password = nullptr;
	return SGX_SUCCESS;
}


int get_service_names()
{
	size_t init_t = 0;
	size_t* count_items = &init_t;
	size_t size_all_services = 0;
	char* vault_titles = nullptr;
	vault_t vault;
	cin.clear();
	cin.ignore(INT_MAX, '\n');
	system("CLS");
	e_get_count_items(get_enclave_id(), count_items, &return_value);
	if (*count_items < 1)
	{
		printf("There are no stored services!\n\n\n");
		return ERROR_EMPTY_VAULT;
	}
	size_all_services = *count_items * sizeof(vault.items->title) + *count_items * 6;
	vault_titles = static_cast<char*>(malloc(size_all_services));
	status = e_get_services_names(get_enclave_id(), vault_titles, size_all_services);
	if (status != SGX_SUCCESS)
	{
		free(vault_titles);
		vault_titles = nullptr;
		return status;
	}
	printf("-------------------------List of all stored services-------------------------\n");
	printf("%s", vault_titles);
	printf("-----------------------------------------------------------------------------\n\n\n");
	free(vault_titles);
	vault_titles = nullptr;

	return SGX_SUCCESS;
}

int get_service_credentials()
{
	const int response = get_service_names();
	if (response == ERROR_EMPTY_VAULT)
	{
		return ERROR_EMPTY_VAULT;
	}
	size_t init_t = 0;
	size_t* count_items = &init_t;

	status = e_get_count_items(get_enclave_id(), count_items, &return_value);

	if (status != SGX_SUCCESS)
	{
		return status;
	}
	const int menu_min = 1;
	item_t* item;
	int* index;
	cin.clear();
	printf("Index of service: ");
	validate_input(static_cast<int>(*count_items), menu_min);
	index = &option_choice;
	item = static_cast<item_t*>(malloc(sizeof(item_t)));
	e_get_credentials(get_enclave_id(), index, item, sizeof(item_t), &return_value);
	if (item != nullptr)
	{
		printf("\n------------------------------------------\n\n");
		printf("Name: %s\n", item->title);
		printf("Username: %s\n", item->username);
		printf("Password: %s\n", item->password);
		printf("\n------------------------------------------\n\n");
		free(item);
		item = nullptr;
	}
	else
	{
		free(item);
		return SGX_ERROR_UNEXPECTED;
	}

	return SGX_SUCCESS;
}

int change_master_password()
{
	char *password, *master_password_new;
	password = new char[MAX_ITEM_SIZE];
	master_password_new = new char[MAX_ITEM_SIZE];
	cin.clear();
	cin.ignore(INT_MAX, '\n');
	system("CLS");
	printf("Type current master password: ");

	master_password = static_cast<char*>(calloc(MAX_ITEM_SIZE, sizeof(char)));
	int password_result;
	password_result = read_password(master_password);
	if (password_result != SGX_SUCCESS)
	{
		handle_password_error(password_result);
	}
	else
	{
		e_verify_master_password(get_enclave_id(), master_password, &return_value);
		if (return_value == SGX_SUCCESS)
		{
			system("CLS");
			printf("Master password has been successfully verified!\n\n");
			printf("Type a new master password: ");
			password_result = read_password(master_password);
			if (password_result != SGX_SUCCESS)
			{
				handle_password_error(password_result);
			}
			else
			{
				printf("\nType again a new master password: ");
				password_result = read_password(master_password_new);
				if (password_result != SGX_SUCCESS)
				{
					handle_password_error(password_result);
				}
				else
				{
					if (strcmp(master_password, master_password_new) == 0)
					{
						e_change_master_password(get_enclave_id(), master_password_new, &return_value);
						if (return_value == SGX_SUCCESS)
						{
							printf("\n\nMaster password has been successfully changed!\n\n");
						}
						else
						{
							printf("\nFailed to change master password!\nPassword policy does not meet!\n\n");
						}
					}
					else
					{
						printf("\nPasswords do not match!\n");
					}
				}
			}
		}
		else
		{
			system("CLS");
			printf("Incorrect password!\n");
		}
	}
	free(master_password);
	master_password = nullptr;
	return SGX_SUCCESS;
}

int remove_vault()
{
	const int menu_max = 1;
	const int menu_min = 0;
	cin.clear();
	cin.ignore(INT_MAX, '\n');
	system("CLS");
	printf("Are you sure? This action is irreversible!\n");
	printf("1 - Yes\n0 - No\n\n");
	printf("Your option: ");
	validate_input(menu_max, menu_min);

	if (option_choice == 1)
	{
		remove_file();
		return SGX_SUCCESS;
	}

	if (option_choice == 0)
	{
		system("CLS");
		printf("You did not remove the vault!\n");
		return -10;
	}

	return SGX_ERROR_UNEXPECTED;
}

int remove_item()
{
	system("CLS");
	const int response = get_service_names();
	if (response == ERROR_EMPTY_VAULT)
	{
		return ERROR_EMPTY_VAULT;
	}

	size_t init_t = 0;
	size_t* count_items = &init_t;

	status = e_get_count_items(get_enclave_id(), count_items, &return_value);

	if (status != SGX_SUCCESS)
	{
		return status;
	}

	const int menu_min = 1;
	int* index;
	cin.clear();
	printf("Index of item that will be deleted: ");
	validate_input(static_cast<int>(*count_items), menu_min);
	index = &option_choice;

	status = e_remove_item(get_enclave_id(), index, &return_value);
	if (status != SGX_SUCCESS)
	{
		return status;
	}
	printf("\n\nItem has been deleted!\n\n");
	return SGX_SUCCESS;
}

int get_option_choice()
{
	return option_choice;
}

void clear_user_interface_memory()
{
	if (master_password != nullptr)
	{
		free(master_password);
		master_password = nullptr;
	}
}
