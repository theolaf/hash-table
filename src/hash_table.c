#include "hash_table.h"

#include <stdlib.h>
#include <string.h>

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
      ht_del_item(item);
    }
  }

  free(ht->items);
  free(ht);
}