#pragma once

typedef struct
{
    char *key;
    char *value;
} KeyValuePair;

typedef struct
{
    int size;
    int count;
    KeyValuePair **items;
} HashTable;

HashTable *new_hash_table();
void delete_hash_table(HashTable *ht);

void insert_kv_pair(HashTable *ht, const char *key, const char *value);
char *search_kv_pair(HashTable *ht, const char *key);
void delete_kv_pair(HashTable *ht, const char *key);