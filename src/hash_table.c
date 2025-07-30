#include "hash_table.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

static KeyValuePair DELETED_ITEM = {NULL, NULL};

static KeyValuePair *new_item(const char *k, const char *v)
{
    KeyValuePair *i = malloc(sizeof(KeyValuePair));

    i->key = strdup(k);
    i->value = strdup(v);

    return i;
}

static void delete_item(KeyValuePair *i)
{
    free(i->key);
    free(i->value);
    free(i);
}

HashTable *new_hash_table()
{
    HashTable *ht = malloc(sizeof(HashTable));

    ht->size = 53;
    ht->count = 0;
    ht->items = calloc((size_t)ht->size, sizeof(KeyValuePair *));

    return ht;
}

void delete_hash_table(HashTable *ht)
{
    for (int i = 0; i < ht->size; i++)
    {
        KeyValuePair *item = ht->items[i];
        if (item && item != &DELETED_ITEM)
        {
            delete_item(item);
        }
    }

    free(ht->items);
    free(ht);
}

void insert_kv_pair(HashTable *ht, const char *key, const char *value)
{
    KeyValuePair *item = new_item(key, value);
    KeyValuePair *current_item;
    int i = 0, index;

    do
    {
        index = get_key_hash(item->key, ht->size, i);
        current_item = ht->items[index];
        i++;

        if (!current_item)
        { // if current item does not exist, no need for the check below
            continue;
        }

        if (strcmp(current_item->key, key) == 0)
        { // removing previous item if key already exist
            delete_item(current_item);
            current_item = NULL;
            ht->count--;
        }
    } while (current_item);

    ht->items[index] = item;
    ht->count++;
}

char *search_kv_pair(HashTable *ht, const char *key)
{
    int index = get_key_hash(key, ht->size, 0);
    KeyValuePair *item = ht->items[index];
    int i = 1;

    while (item)
    {
        if (strcmp(item->key, key) == 0)
        {
            return item->value;
        }

        index = get_key_hash(key, ht->size, i);
        item = ht->items[index];
        i++;
    }

    return NULL;
}

void delete_kv_pair(HashTable *ht, const char *key)
{
    int index = get_key_hash(key, ht->size, 0);
    KeyValuePair *item = ht->items[index];
    int i = 1;
    bool key_found = false;

    while (item)
    {
        if (strcmp(item->key, key) == 0)
        {
            delete_item(item);
            ht->items[index] = &DELETED_ITEM;
            key_found = true;
        }

        index = get_key_hash(key, ht->size, i);
        item = ht->items[index];
        i++;
    }

    if (key_found)
    {
        ht->count--;
    }
}