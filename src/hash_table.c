#include "hash_table.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

static kv_pair_t DELETED_ITEM = {NULL, NULL};

static kv_pair_t *create_kv_pair(const char *k, const char *v)
{
    kv_pair_t *i = malloc(sizeof(kv_pair_t));

    i->key = strdup(k);
    i->value = strdup(v);

    return i;
}

static void delete_kv_pair(kv_pair_t *i)
{
    free(i->key);
    free(i->value);
    free(i);
}

hash_table_t *create_hash_table()
{
    hash_table_t *ht = malloc(sizeof(hash_table_t));

    ht->size = 53;
    ht->count = 0;
    ht->items = calloc((size_t)ht->size, sizeof(kv_pair_t *));

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
    kv_pair_t *current_item;
    int i = 0, index;

    do
    {
        index = get_hash(item->key, ht->size, i);
        current_item = ht->items[index];
        i++;

        if (!current_item)
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
}

char *hash_table_search(hash_table_t *ht, const char *key)
{
    int index = get_hash(key, ht->size, 0);
    kv_pair_t *item = ht->items[index];
    int i = 1;

    while (item)
    {
        if (strcmp(item->key, key) == 0)
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
        if (strcmp(item->key, key) == 0)
        {
            delete_kv_pair(item);
            ht->items[index] = &DELETED_ITEM;
            key_found = true;
        }

        index = get_hash(key, ht->size, i);
        item = ht->items[index];
        i++;
    }

    if (key_found)
    {
        ht->count--;
    }
}