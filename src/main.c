#include <stdio.h>

#include "hash_table.h"

int main()
{
    printf("Creating empty hash table...\n");
    hash_table *ht = new_hash_table();
    printf("Table size: %d\n", ht->size);
    printf("Table count: %d\n", ht->count);

    printf("Inserting a new item {hello: world}...\n");
    insert_kv_pair(ht, "hello", "world");

    printf("Replacing the value for existing key {hello: bye}...\n");
    insert_kv_pair(ht, "hello", "bye");

    printf("Deleting the hash table...\n");
    delete_hash_table(ht);
    return 0;
}