#include "Sealing.h"

sgx_status_t seal_vault(vault_t* plaintext, sgx_sealed_data_t* sealed_data, uint32_t sealed_size)
{
	return sgx_seal_data(0, nullptr, sizeof(vault_t), reinterpret_cast<uint8_t*>(plaintext), sealed_size, sealed_data);
}

sgx_status_t unseal_vault(const sgx_sealed_data_t* sealed_data, vault_t* plaintext, uint32_t plaintext_size)
{
	return sgx_unseal_data(sealed_data, nullptr, nullptr, reinterpret_cast<uint8_t*>(plaintext), &plaintext_size);
}
