#include "hash_table.h"

#include <stdlib.h>
#include <string.h>

#include "utils.h"

static kv_pair* new_item(const char* k, const char* v) {
  kv_pair* i = malloc(sizeof(kv_pair));

  i->key = strdup(k);
  i->value = strdup(v);

  return i;
}

static void delete_item(kv_pair* i) {
  free(i->key);
  free(i->value);
  free(i);
}

hash_table* new_hash_table() {
  hash_table* ht = malloc(sizeof(hash_table));

  ht->size = 53;
  ht->count = 0;
  ht->items = calloc((size_t)ht->size, sizeof(kv_pair*));

  return ht;
}

void delete_hash_table(hash_table* ht) {
  for (int i = 0; i < ht->size; i++) {
    kv_pair* item = ht->items[i];
    if (item != NULL) {
      delete_item(item);
    }
  }

  free(ht->items);
  free(ht);
}

void ht_insert(hash_table* ht, const char* key, const char* value) {
  kv_pair* item = new_item(key, value);
  kv_pair* current_item;
  int i = 0, index;

  do {
    index = get_key_hash(item->key, ht->size, i);
    current_item = ht->items[index];
    i++;
  } while (current_item != NULL);

  ht->items[index] = item;
  ht->count++;
}