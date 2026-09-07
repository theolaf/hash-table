#pragma once

#include <stdlib.h>

// String-specific hash (backward compatible)
int get_hash(const char *key, int num_buckets, int retries);

// Generic hash function type
unsigned long hash_string(void *key, int num_buckets, int retries);
unsigned long hash_int(void *key, int num_buckets, int retries);

// Comparison functions
int compare_string(void *a, void *b);
int compare_int(void *a, void *b);

// Prime utilities
int next_prime(int x);
