#include "Enclave.h"

sgx_sealed_data_t* sealed_data = nullptr;
vault_t* unsealed_data = nullptr;
sgx_status_t sgx_status;
uint32_t sealed_size = 0;
uint32_t unsealed_data_size = 0;
uint32_t data_size;
vault_t vault;
size_t sealed_data_size;
int o_return_value;

void e_create_vault(char* master_password, const int* is_password, int* return_value)
{
	if (*is_password == 1)
	{
		if (validate_master_password(master_password) == 1)
		{
			*return_value = ERROR_PASSWORD_POLICY_FAILED;
			return;
		}
		strcpy_s(vault.master_password, MAX_ITEM_SIZE, master_password);
	}
	else
	{
		vault.master_password[0] = '\0';
	}

	vault.size = 0;
	data_size = sizeof(vault);
	sgx_status = data_sealing();

	if (sgx_status != SGX_SUCCESS)
	{
		clear_unsealed_data();
		*return_value = ERROR_FAIL_SEAL;
	}

	sgx_status = save_vault();
	free(sealed_data);
	if (o_return_value != SGX_SUCCESS || sgx_status != SGX_SUCCESS)
	{
		clear_unsealed_data();
		*return_value = ERROR_FAIL_SAVE_VAULT;
	}
	else
	{
		clear_unsealed_data();
		*return_value = SGX_SUCCESS;
	}
}

void e_unlock_vault(char* master_password, int* return_value)
{
	if (verify_master_password(master_password) == ERROR_PASSWORDS_DO_NOT_MATCH)
	{
		*return_value = ERROR_PASSWORD_POLICY_FAILED;
		return;
	}
	clear_unsealed_data();
	*return_value = SGX_SUCCESS;
}

void e_is_password(int* is_password, int* return_value)
{
	sgx_status = read_vault();
	o_return_value = data_unsealing();
	if (o_return_value != SGX_SUCCESS)
	{
		*return_value = o_return_value;
		return;
	}

	if (passwords_match(unsealed_data->master_password, "") == SGX_SUCCESS)
	{
		clear_unsealed_data();
		*is_password = FAILURE;
		return;
	}
	*is_password = SGX_SUCCESS;
}

void e_verify_master_password(char* master_password, int* return_value)
{
	o_return_value = data_unsealing();
	if (o_return_value != SGX_SUCCESS)
	{
		*return_value = o_return_value;
		return;
	}
	if (passwords_match(unsealed_data->master_password, master_password) != SGX_SUCCESS)
	{
		*return_value = ERROR_PASSWORDS_DO_NOT_MATCH;
		clear_unsealed_data();
		return;
	}
	clear_unsealed_data();
	*return_value = SGX_SUCCESS;
}

void e_add_service(char* service_name, char* username, char* password, int* return_value)
{
	if (validate_service_password(password) == FAILURE)
	{
		*return_value = ERROR_PASSWORD_POLICY_FAILED;
		return;
	}
	sgx_status = data_unsealing();
	if (sgx_status != SGX_SUCCESS)
	{
		*return_value = sgx_status;
		clear_unsealed_data();
		return;
	}
	vault = *unsealed_data;
	const auto size = vault.size;
	strcpy_s(vault.items[size].title, MAX_ITEM_SIZE, service_name);
	strcpy_s(vault.items[size].username, MAX_ITEM_SIZE, username);
	strcpy_s(vault.items[size].password, MAX_ITEM_SIZE, password);
	vault.size++;
	unsealed_data = &vault;
	sgx_status = save_vault();
	unsealed_data = nullptr;

	if (sgx_status != SGX_SUCCESS)
	{
		*return_value = sgx_status;
		return;
	}
	*return_value = SGX_SUCCESS;
}

void e_get_services_names(char* services_names, size_t len)
{
	size_t length;
	sgx_status = data_unsealing();
	if (sgx_status != SGX_SUCCESS)
	{
		clear_unsealed_data();
		return;
	}
	vault = *unsealed_data;
	const size_t vault_size = vault.size;
	char* temp_buffer = static_cast<char*>(malloc(len));
	temp_buffer[0] = '\0';
	for (size_t i = 0; i < vault_size; i++)
	{
		length = strlen(temp_buffer);
		_snprintf_s(temp_buffer + length, len, strlen(vault.items[i].title) + 6, "%llu: %s\n", i + 1,
		            vault.items[i].title);
	}
	memcpy_s(services_names, len, temp_buffer, strlen(temp_buffer) + 1);
	free(temp_buffer);
	temp_buffer = nullptr;
	clear_unsealed_data();
}

void e_remove_item(int* index, int* return_value)
{
	sgx_status = data_unsealing();
	if (sgx_status != SGX_SUCCESS)
	{
		clear_unsealed_data();
		*return_value = sgx_status;
		return;
	}
	vault = *unsealed_data;
	const auto correct_index = *index - 1;
	const auto vault_size = vault.size;
	char empty_char[MAX_ITEM_SIZE];
	empty_char[0] = '\0';

	for (auto i = correct_index; static_cast<size_t>(i) < vault_size; i++)
	{
		if ((static_cast<size_t>(i) + 1) <= vault_size - 1)
		{
			vault.items[i] = vault.items[i + 1];
		}
	}
	strcpy_s(vault.items[vault_size - 1].title, MAX_ITEM_SIZE, empty_char);
	strcpy_s(vault.items[vault_size - 1].username, MAX_ITEM_SIZE, empty_char);
	strcpy_s(vault.items[vault_size - 1].password, MAX_ITEM_SIZE, empty_char);

	vault.size--;
	unsealed_data = &vault;
	sgx_status = save_vault();
	unsealed_data = nullptr;

	if (sgx_status != SGX_SUCCESS)
	{
		*return_value = sgx_status;
		return;
	}
	*return_value = SGX_SUCCESS;
}

void e_get_credentials(int* index, item_t* item, size_t item_size, int* return_value)
{
	sgx_status = data_unsealing();
	if (sgx_status != SGX_SUCCESS)
	{
		clear_unsealed_data();
		*return_value = sgx_status;
		return;
	}

	item_t* vault_item = nullptr;
	item_t new_item;
	vault = *unsealed_data;
	size_t size = vault.size;
	const int correct_index = *index - 1;

	strcpy_s(new_item.title, MAX_ITEM_SIZE, vault.items[correct_index].title);
	strcpy_s(new_item.username, MAX_ITEM_SIZE, vault.items[correct_index].username);
	strcpy_s(new_item.password, MAX_ITEM_SIZE, vault.items[correct_index].password);

	vault_item = &new_item;

	memcpy_s(item->title, MAX_ITEM_SIZE, vault_item->title, strlen(new_item.title) + 1);
	memcpy_s(item->username, MAX_ITEM_SIZE, vault_item->username, strlen(new_item.username) + 1);
	memcpy_s(item->password, MAX_ITEM_SIZE, vault_item->password, strlen(new_item.password) + 1);
	clear_unsealed_data();
	vault_item = nullptr;
	*return_value = SGX_SUCCESS;
}

void e_change_master_password(char* master_password_new, int* return_value)
{
	if (validate_master_password(master_password_new) == 1)
	{
		*return_value = ERROR_PASSWORD_POLICY_FAILED;
		return;
	}

	sgx_status = data_unsealing();
	if (sgx_status != SGX_SUCCESS)
	{
		clear_unsealed_data();
		*return_value = ERROR_FAIL_UNSEAL;
		return;
	}

	vault = *unsealed_data;
	strcpy_s(vault.master_password, MAX_ITEM_SIZE, master_password_new);
	unsealed_data = &vault;
	sgx_status = save_vault();
	if (sgx_status != SGX_SUCCESS)
	{
		*return_value = sgx_status;
		unsealed_data = nullptr;
		return;
	}
	*return_value = SGX_SUCCESS;
	unsealed_data = nullptr;
}

void e_get_count_items(size_t* count_items, int* return_value)
{
	sgx_status = data_unsealing();
	if (sgx_status != SGX_SUCCESS)
	{
		*return_value = sgx_status;
		clear_unsealed_data();
		return;
	}
	*count_items = unsealed_data->size;
	clear_unsealed_data();
}

sgx_status_t data_sealing()
{
	data_size = sizeof(vault);
	sealed_size = sgx_calc_sealed_data_size(NULL, data_size);

	if (sealed_size == 0 || sealed_size == UINT32_MAX)
	{
		return SGX_ERROR_UNEXPECTED;
	}

	sealed_data = static_cast<sgx_sealed_data_t*>(malloc(sealed_size));
	sgx_status = seal_vault(&vault, sealed_data, sealed_size);
	return sgx_status;
}

sgx_status_t data_unsealing()
{
	unsealed_data = static_cast<vault_t*>(malloc(unsealed_data_size));
	auto* tmp = static_cast<sgx_sealed_data_t*>(malloc(sealed_data_size));
	memcpy_s(tmp, sealed_data_size, sealed_data, sealed_data_size);
	sgx_status = unseal_vault(tmp, unsealed_data, unsealed_data_size);
	free(tmp);
	tmp = nullptr;
	return sgx_status;
}

sgx_status_t read_vault()
{
	sgx_status = o_read_vault(reinterpret_cast<unsigned char**>(&sealed_data), &sealed_data_size, &o_return_value);
	if (sgx_status != SGX_SUCCESS || o_return_value != 0)
	{
		return sgx_status;
	}
	unsealed_data_size = sgx_get_encrypt_txt_len(sealed_data);
	if (unsealed_data_size == 0 || unsealed_data_size == UINT32_MAX)
	{
		return SGX_ERROR_UNEXPECTED;
	}
	return SGX_SUCCESS;
}

sgx_status_t save_vault()
{
	sgx_status = data_sealing();
	if (sgx_status != SGX_SUCCESS)
	{
		return sgx_status;
	}
	sgx_status = o_save_vault(reinterpret_cast<unsigned char*>(sealed_data), sealed_size, &o_return_value);
	if (sgx_status != SGX_SUCCESS || o_return_value != 0)
	{
		return sgx_status;
	}
	return SGX_SUCCESS;
}


int verify_master_password(char* master_password)
{
	if (passwords_match(unsealed_data->master_password, master_password) != 0)
	{
		return ERROR_PASSWORDS_DO_NOT_MATCH;
	}
	return SGX_SUCCESS;
}

void clear_unsealed_data()
{
	if (unsealed_data != nullptr)
	{
		free(unsealed_data);
		unsealed_data = nullptr;
	}
}

void e_clear_enclave_memory(int* return_value)
{
	clear_unsealed_data();
	if (sealed_data != nullptr)
	{
		sealed_data = nullptr;
	}
	sealed_size = 0;
	unsealed_data_size = 0;
	data_size = 0;
	sealed_data_size = 0;
	*return_value = SGX_SUCCESS;
}
