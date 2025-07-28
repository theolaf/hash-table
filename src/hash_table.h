typedef struct {
    char* key;
    char* value;
} kv_pair;

typedef struct {
    int size;
    int count;
    kv_pair** items;
} hash_table;

hash_table* ht_new();

void delete_ht_hash_table(hash_table* ht);