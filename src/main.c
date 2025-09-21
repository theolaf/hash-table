#include <stdio.h>

#include "hash_table.h"

static const int BUFFER_SIZE = 3;

int main()
{
    printf("Creating empty hash table...\n");
    hash_table_t *ht = create_hash_table(HT_BASE_SIZE);
    printf("Table size: %d\n", ht->size);
    printf("Table count: %d\n", ht->count);

    printf("Inserting a new item {hello: world}...\n");
    hash_table_insert(ht, "hello", "world");

    printf("The value for key 'hello' is: %s\n", hash_table_search(ht, "hello"));

    printf("Replacing the value for existing key {hello: bye}...\n");
    hash_table_insert(ht, "hello", "bye");

    printf("The value for key 'hello' is now: %s\n", hash_table_search(ht, "hello"));

    printf("Deleting the item for key 'hello'...\n");
    hash_table_remove(ht, "hello");
    printf("New count value: %d\n", ht->count);
    printf("New size value: %d\n", ht->size);

    printf("Adding 33 elements to showcase upsizing...\n");
    for (int i = 0; i < 33; i++)
    {
        char key[BUFFER_SIZE];
        sprintf(key, "%d", i);
        hash_table_insert(ht, key, "value");
    }
    printf("New count value: %d\n", ht->count);
    printf("New size value: %d\n", ht->size);

    printf("Removing 24 elements to showcase downsizing...\n");
    for (int i = 0; i < 24; i++)
    {
        char key[BUFFER_SIZE];
        sprintf(key, "%d", i);
        hash_table_remove(ht, key);
    }
    printf("New count value: %d\n", ht->count);
    printf("New size value: %d\n", ht->size);

    printf("Deleting the hash table...\n");
    delete_hash_table(ht);
    return 0;
}