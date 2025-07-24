typedef struct {
    char* key;
    char* value;
} kv_pair;

typedef struct {
    int size;
    int count;
    kv_pair** items;
} hash_table;