#pragma once
#define MAX_ITEMS 100
#define MAX_ITEM_SIZE 30

struct Item {
	char  title[MAX_ITEM_SIZE];
	char  username[MAX_ITEM_SIZE];
	char  password[MAX_ITEM_SIZE];
};
typedef struct Item item_t;

struct Vault {
	item_t items[MAX_ITEMS];
	size_t size;
	char master_password[MAX_ITEM_SIZE];
};
typedef struct Vault vault_t;