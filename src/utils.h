static int hash(const char* string, const int prime, const int num_buckets);

int get_key_hash(const char* key, const int num_buckets, const int retries);