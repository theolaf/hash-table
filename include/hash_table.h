#pragma once

typedef struct
{
    char *key;
    char *value;
} kv_pair;

typedef struct
{
    int size;
    int count;
    kv_pair **items;
} hash_table;

hash_table *new_hash_table();
void delete_hash_table(hash_table *ht);

void insert_kv_pair(hash_table *ht, const char *key, const char *value);
char *search_kv_pair(hash_table *ht, const char *key);
void delete_kv_pair(hash_table *ht, const char *key);