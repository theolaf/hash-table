#pragma once

typedef struct KVPair kv_pair_t;
typedef struct HashTable hash_table_t;

struct KVPair
{
    char *key;
    char *value;
};

struct HashTable
{
    int size;
    int count;
    kv_pair_t **items;
};

hash_table_t *create_hash_table();
void delete_hash_table(hash_table_t *ht);

void hash_table_insert(hash_table_t *ht, const char *key, const char *value);
char *hash_table_search(hash_table_t *ht, const char *key);
void hash_table_remove(hash_table_t *ht, const char *key);