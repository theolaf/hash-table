#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash_table.h"

static const int BUFFER_SIZE = 3;

// Helper to create a copy of an integer
int *int_copy(int value) {
    int *copy = malloc(sizeof(int));
    if (copy) {
        *copy = value;
    }
    return copy;
}

// Helper to create a copy of a string
char *str_copy(const char *str) {
    return strdup(str);
}

int main()
{
    // ============================================
    // Part 1: Test string hash table (existing functionality)
    // ============================================
    printf("\n=== STRING HASH TABLE TEST ===\n");
    
    printf("Creating empty string hash table...\n");
    hash_table_t *str_ht = create_hash_table(HT_BASE_SIZE);
    printf("Table size: %d\n", str_ht->size);
    printf("Table count: %d\n", str_ht->count);

    printf("Inserting a new item {hello: world}...\n");
    hash_table_insert(str_ht, str_copy("hello"), str_copy("world"));

    char *result = (char *)hash_table_search(str_ht, "hello");
    printf("The value for key 'hello' is: %s\n", result);

    printf("Replacing the value for existing key {hello: bye}...\n");
    hash_table_insert(str_ht, str_copy("hello"), str_copy("bye"));

    result = (char *)hash_table_search(str_ht, "hello");
    printf("The value for key 'hello' is now: %s\n", result);

    printf("Deleting the item for key 'hello'...\n");
    hash_table_remove(str_ht, "hello");
    printf("New count value: %d\n", str_ht->count);
    printf("New size value: %d\n", str_ht->size);

    printf("Adding 33 elements to showcase upsizing...\n");
    for (int i = 0; i < 33; i++)
    {
        char key[BUFFER_SIZE];
        sprintf(key, "%d", i);
        hash_table_insert(str_ht, str_copy(key), str_copy("value"));
    }
    printf("New count value: %d\n", str_ht->count);
    printf("New size value: %d\n", str_ht->size);

    printf("Removing 24 elements to showcase downsizing...\n");
    for (int i = 0; i < 24; i++)
    {
        char key[BUFFER_SIZE];
        sprintf(key, "%d", i);
        hash_table_remove(str_ht, key);
    }
    printf("New count value: %d\n", str_ht->count);
    printf("New size value: %d\n", str_ht->size);

    printf("Deleting the string hash table...\n");
    delete_hash_table(str_ht);

    // ============================================
    // Part 2: Test integer hash table (new generic functionality)
    // ============================================
    printf("\n=== INTEGER HASH TABLE TEST ===\n");
    
    printf("Creating empty integer hash table...\n");
    hash_table_t *int_ht = create_int_hash_table(HT_BASE_SIZE);
    printf("Table size: %d\n", int_ht->size);
    printf("Table count: %d\n", int_ht->count);

    printf("Inserting integer items...\n");
    for (int i = 0; i < 10; i++) {
        int *key = int_copy(i);
        int *value = int_copy(i * 10);
        hash_table_insert(int_ht, key, value);
    }
    printf("Inserted 10 integer items\n");
    printf("Count: %d\n", int_ht->count);

    printf("Testing search for key 5...\n");
    int search_key = 5;
    int *found_value = (int *)hash_table_search(int_ht, &search_key);
    if (found_value) {
        printf("Found value for key 5: %d\n", *found_value);
    } else {
        printf("Key 5 not found\n");
    }

    printf("Testing search for non-existent key 99...\n");
    int not_found_key = 99;
    int *not_found = (int *)hash_table_search(int_ht, &not_found_key);
    if (not_found == NULL) {
        printf("Key 99 correctly not found\n");
    } else {
        printf("ERROR: Key 99 should not exist\n");
    }

    printf("Updating value for key 3 to 999...\n");
    int *key_3 = int_copy(3);
    int *value_999 = int_copy(999);
    hash_table_insert(int_ht, key_3, value_999);
    int search_3 = 3;
    int *updated_value = (int *)hash_table_search(int_ht, &search_3);
    printf("Value for key 3 is now: %d\n", *updated_value);

    printf("Removing key 7...\n");
    int remove_key = 7;
    hash_table_remove(int_ht, &remove_key);
    printf("Count after removal: %d\n", int_ht->count);
    
    int search_7 = 7;
    int *should_be_null = (int *)hash_table_search(int_ht, &search_7);
    if (should_be_null == NULL) {
        printf("Key 7 successfully removed\n");
    } else {
        printf("ERROR: Key 7 should have been removed\n");
    }

    printf("Adding 30 more elements to trigger upsizing...\n");
    for (int i = 10; i < 40; i++) {
        int *key = int_copy(i);
        int *value = int_copy(i * 100);
        hash_table_insert(int_ht, key, value);
    }
    printf("New count: %d\n", int_ht->count);
    printf("New size: %d\n", int_ht->size);

    printf("Removing 25 elements to trigger downsizing...\n");
    for (int i = 0; i < 25; i++) {
        int remove_key = i;
        hash_table_remove(int_ht, &remove_key);
    }
    printf("Final count: %d\n", int_ht->count);
    printf("Final size: %d\n", int_ht->size);

    printf("Deleting the integer hash table...\n");
    delete_hash_table(int_ht);

    printf("\nAll tests completed successfully!\n");
    return 0;
}
