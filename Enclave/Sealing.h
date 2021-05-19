#pragma once
#ifndef SEALING_H_
#define SEALING_H_
#include "Vault.h"
#include "sgx_trts.h"
#include "sgx_tseal.h"
#include "Enclave_t.h"
#include "sgx_trts.h"
#include "sgx_tseal.h"

sgx_status_t seal_vault(vault_t* plaintext, sgx_sealed_data_t* sealed_data, uint32_t sealed_size);
sgx_status_t unseal_vault(const sgx_sealed_data_t* sealed_data, vault_t* plaintext, uint32_t plaintext_size);
#endif