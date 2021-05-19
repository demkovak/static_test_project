#include "PasswordHandling.h"

int validate_master_password(char* master_password)
{
	if (strlen(master_password) < MASTER_PASSWORD_MIN_LENGTH || strlen(master_password) + 1 > MAX_ITEM_SIZE)
	{
		return EXIT_FAILURE;
	}
	auto found_lower = false, found_upper = false, is_digit = false;
	for (int i = 0; master_password[i] != '\0'; i++)
	{
		if (isdigit(master_password[i]) != 0)
		{
			is_digit = true;
		}

		else if (master_password[i] >= 'a' && master_password[i] <= 'z')
		{
			found_lower = true;
		}

		else if (master_password[i] >= 'A' && master_password[i] <= 'Z')
		{
			found_upper = true;
		}
		if (found_lower && found_upper && is_digit)
		{
			return EXIT_SUCCESS;
		}
	}
	return EXIT_FAILURE;
}

int validate_service_password(char* service_password)
{
	if (strlen(service_password) < SERVICE_PASSWORD_MIN_LENGTH || strlen(service_password) + 1 > MAX_ITEM_SIZE)
	{
		return EXIT_FAILURE;
	}
	auto is_digit = false, is_letter = false;
	for (int i = 0; service_password[i] != '\0'; i++)
	{
		if (isdigit(service_password[i]) != 0)
		{
			is_digit = true;
		}
		else if (isalpha(service_password[i]) != 0)
		{
			is_letter = true;
		}

		if (is_digit && is_letter)
		{
			return EXIT_SUCCESS;
		}
	}
	return EXIT_FAILURE;
}

int passwords_match(const char* first_password, const char* second_password)
{
	if (strcmp(first_password, second_password) == 0)
	{
		return EXIT_SUCCESS;
	}
	return EXIT_FAILURE;
}
