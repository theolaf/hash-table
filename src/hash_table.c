#include "hash_table.h"
#include "utils.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static kv_pair_t DELETED_ITEM = {NULL, NULL};
static const float UPSIZE_TRIGGER = .7;
static const float DOWNSIZE_TRIGGER = .1;
const int HT_BASE_SIZE = 47;

static kv_pair_t *create_kv_pair(void *k, void *v)
{
    kv_pair_t *i = malloc(sizeof(kv_pair_t));
    if (i == NULL) {
        return NULL;
    }
    i->key = k;
    i->value = v;
    return i;
}

static void delete_kv_pair(hash_table_t *ht, kv_pair_t *i)
{
    if (i) {
        if (ht->key_free && i->key) {
            ht->key_free(i->key);
        }
        if (ht->value_free && i->value) {
            ht->value_free(i->value);
        }
        free(i);
    }
}

static void resize_hash_table(hash_table_t *ht, const int size)
{
    if (size < HT_BASE_SIZE)
    {
        return;
    }

    hash_table_t *new_ht = create_generic_hash_table(
        size,
        ht->hash_func,
        ht->compare_func,
        ht->key_free,
        ht->value_free
    );
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

    // Free only the struct, not the items (they were moved to ht)
    free(new_ht);
}

hash_table_t *create_generic_hash_table(
    int size,
    hash_func_t hash_func,
    compare_func_t compare_func,
    void (*key_free)(void *),
    void (*value_free)(void *)
)
{
    hash_table_t *ht = malloc(sizeof(hash_table_t));
    if (ht == NULL) {
        return NULL;
    }

    ht->size = next_prime(size);
    ht->count = 0;
    ht->hash_func = hash_func;
    ht->compare_func = compare_func;
    ht->key_free = key_free;
    ht->value_free = value_free;

    ht->items = calloc((size_t)ht->size, sizeof(kv_pair_t *));
    if (ht->items == NULL) {
        free(ht);
        return NULL;
    }

    return ht;
}

// Backward-compatible string constructor
hash_table_t *create_hash_table(int size)
{
    return create_generic_hash_table(
        size,
        (hash_func_t)hash_string,
        compare_string,
        free,
        free
    );
}

// Integer constructor
hash_table_t *create_int_hash_table(int size)
{
    return create_generic_hash_table(
        size,
        hash_int,
        compare_int,
        free,
        free
    );
}

void delete_hash_table(hash_table_t *ht)
{
    if (ht == NULL) return;

    for (int i = 0; i < ht->size; i++)
    {
        kv_pair_t *item = ht->items[i];
        if (item && item != &DELETED_ITEM)
        {
            delete_kv_pair(ht, item);
        }
    }

    free(ht->items);
    free(ht);
}

void hash_table_insert(hash_table_t *ht, void *key, void *value)
{
    kv_pair_t *item = create_kv_pair(key, value);
    if (item == NULL) {
        return;
    }

    int i = 0, index;
    kv_pair_t *current_item;

    do
    {
        index = (int)ht->hash_func(key, ht->size, i);
        current_item = ht->items[index];
        i++;

        if (!current_item || current_item == &DELETED_ITEM)
        {
            continue;
        }

        // Use callback to compare keys
        if (ht->compare_func(current_item->key, key) == 0)
        {
            // Key already exists, replace it
            delete_kv_pair(ht, current_item);
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

void *hash_table_search(hash_table_t *ht, void *key)
{
    int index = (int)ht->hash_func(key, ht->size, 0);
    kv_pair_t *item = ht->items[index];
    int i = 1;

    while (item)
    {
        if (item != &DELETED_ITEM && ht->compare_func(item->key, key) == 0)
        {
            return item->value;
        }

        index = (int)ht->hash_func(key, ht->size, i);
        item = ht->items[index];
        i++;
    }

    return NULL;
}

void hash_table_remove(hash_table_t *ht, void *key)
{
    int index = (int)ht->hash_func(key, ht->size, 0);
    kv_pair_t *item = ht->items[index];
    int i = 1;
    bool key_found = false;

    while (item)
    {
        if (item != &DELETED_ITEM && ht->compare_func(item->key, key) == 0)
        {
            delete_kv_pair(ht, item);
            ht->items[index] = &DELETED_ITEM;
            key_found = true;
            break;
        }

        index = (int)ht->hash_func(key, ht->size, i);
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
