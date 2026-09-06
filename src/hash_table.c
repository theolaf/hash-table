#include "hash_table.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

static kv_pair_t DELETED_ITEM = {NULL, NULL};
static const float UPSIZE_TRIGGER = .7;
static const float DOWNSIZE_TRIGGER = .1;
const int HT_BASE_SIZE = 47;

static kv_pair_t *create_kv_pair(const char *k, const char *v)
{
    // Allocate memory for the KVPair struct
    kv_pair_t *i = malloc(sizeof(kv_pair_t));
    if (i == NULL) {
        return NULL;  // malloc failed
    }

    // Allocate and copy the key string
    i->key = strdup(k);
    if (i->key == NULL) {
        free(i);  // Clean up struct if strdup fails
        return NULL;
    }

    // Allocate and copy the value string
    i->value = strdup(v);
    if (i->value == NULL) {
        free(i->key);  // Clean up key
        free(i);        // Clean up struct
        return NULL;
    }

    return i;
}

static void delete_kv_pair(kv_pair_t *i)
{
    free(i->key);
    free(i->value);
    free(i);
}

static void resize_hash_table(hash_table_t *ht, const int size)
{
    if (size < HT_BASE_SIZE)
    {
        return;
    }

    hash_table_t *new_ht = create_hash_table(size);
    // If allocation failed, keep using the current table
    if (new_ht == NULL) {
        return;
    }

    for (int i = 0; i < ht->size; i++)
    {
        kv_pair_t *item = ht->items[i];
        if (item && item != &DELETED_ITEM)
        {
            hash_table_insert(new_ht, item->key, item->value);
        }
    }

    int tmp_size = new_ht->size;
    new_ht->size = ht->size;
    ht->size = tmp_size;

    kv_pair_t **tmp_items = new_ht->items;
    new_ht->items = ht->items;
    ht->items = tmp_items;

    delete_hash_table(new_ht);
}

hash_table_t *create_hash_table(const int size)
{
    // Allocate memory for the HashTable struct
    hash_table_t *ht = malloc(sizeof(hash_table_t));
    if (ht == NULL) {
        return NULL;  // malloc failed
    }

    ht->size = next_prime(size);
    ht->count = 0;

    // Allocate and zero-initialize the items array
    ht->items = calloc((size_t)ht->size, sizeof(kv_pair_t *));
    if (ht->items == NULL) {
        free(ht);  // Clean up table if calloc fails
        return NULL;
    }

    return ht;
}

void delete_hash_table(hash_table_t *ht)
{
    for (int i = 0; i < ht->size; i++)
    {
        kv_pair_t *item = ht->items[i];
        if (item && item != &DELETED_ITEM)
        {
            delete_kv_pair(item);
        }
    }

    free(ht->items);
    free(ht);
}

void hash_table_insert(hash_table_t *ht, const char *key, const char *value)
{
    kv_pair_t *item = create_kv_pair(key, value);
    // If allocation failed, we cannot insert
    if (item == NULL) {
        return;  // Silently fail - caller can retry or handle error
    }
    kv_pair_t *current_item;
    int i = 0, index;

    do
    {
        index = get_hash(item->key, ht->size, i);
        current_item = ht->items[index];
        i++;

        if (!current_item || current_item == &DELETED_ITEM)
        { // if current item does not exist, no need for the check below
            continue;
        }

        if (strcmp(current_item->key, key) == 0)
        { // removing previous item if key already exist
            delete_kv_pair(current_item);
            current_item = NULL;
            ht->count--;
        }
    } while (current_item);

    ht->items[index] = item;
    ht->count++;

    float load = (float)ht->count / ht->size;

    if (load > UPSIZE_TRIGGER)
    {
        resize_hash_table(ht, ht->size * 2);
    }
}

char *hash_table_search(hash_table_t *ht, const char *key)
{
    int index = get_hash(key, ht->size, 0);
    kv_pair_t *item = ht->items[index];
    int i = 1;

    while (item)
    {
        if (item != &DELETED_ITEM && strcmp(item->key, key) == 0)
        {
            return item->value;
        }

        index = get_hash(key, ht->size, i);
        item = ht->items[index];
        i++;
    }

    return NULL;
}

void hash_table_remove(hash_table_t *ht, const char *key)
{
    int index = get_hash(key, ht->size, 0);
    kv_pair_t *item = ht->items[index];
    int i = 1;
    bool key_found = false;

    while (item)
    {
        if (item != &DELETED_ITEM && strcmp(item->key, key) == 0)
        {
            delete_kv_pair(item);
            ht->items[index] = &DELETED_ITEM;
            key_found = true;
            break;
        }

        index = get_hash(key, ht->size, i);
        item = ht->items[index];
        i++;
    }

    if (key_found)
    {
        ht->count--;
    }

    float load = (float)ht->count / ht->size;

    if (load < DOWNSIZE_TRIGGER)
    {
        resize_hash_table(ht, ht->size / 2);
    }
}
