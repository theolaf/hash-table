#include "utils.h"

#include <math.h>
#include <string.h>

static int hash(const char *string, const int prime, const int num_buckets)
{
    long hashed = 0;
    const int string_length = strlen(string);

    for (int i = 0; i < string_length; i++)
    {
        hashed += (long)pow(prime, string_length - (i + 1)) * string[i];
    }

    return (int)(hashed % num_buckets);
}

int get_hash(const char *key, const int num_buckets, const int retries)
{
    const int hashed_a = hash(key, 151, num_buckets);
    const int hashed_b = hash(key, 163, num_buckets);
    return (hashed_a + (retries * (hashed_b + 1))) % num_buckets;
}