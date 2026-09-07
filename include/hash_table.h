#pragma once

#include <stdlib.h>

typedef struct KVPair kv_pair_t;
typedef struct HashTable hash_table_t;

// Function pointer types for generic type support
typedef unsigned long (*hash_func_t)(void *key, int num_buckets, int retries);
typedef int (*compare_func_t)(void *a, void *b);

struct KVPair
{
    void *key;
    void *value;
};

struct HashTable
{
    int size;
    int count;
    kv_pair_t **items;

    // Type-specific callbacks
    hash_func_t hash_func;
    compare_func_t compare_func;
    void (*key_free)(void *key);
    void (*value_free)(void *value);
};

extern const int HT_BASE_SIZE;

// Main generic constructor
hash_table_t *create_generic_hash_table(
    int size,
    hash_func_t hash_func,
    compare_func_t compare_func,
    void (*key_free)(void *),
    void (*value_free)(void *)
);

// Backward-compatible string constructor
hash_table_t *create_hash_table(int size);

// Convenience constructor for integers
hash_table_t *create_int_hash_table(int size);

// Cleanup
void delete_hash_table(hash_table_t *ht);

// Insert, search, remove with void* for generic support
void hash_table_insert(hash_table_t *ht, void *key, void *value);
void *hash_table_search(hash_table_t *ht, void *key);
void hash_table_remove(hash_table_t *ht, void *key);
