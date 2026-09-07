#include "utils.h"

#include <math.h>
#include <stdbool.h>
#include <string.h>

static int hash(const char *string, const int prime, const int num_buckets)
{
    // Use rolling hash to avoid overflow from pow()
    // Equivalent to: Σ(prime^(length-i-1) * string[i]) % num_buckets
    // But computed iteratively to prevent integer overflow
    long hashed = 0;
    const int string_length = strlen(string);

    for (int i = 0; i < string_length; i++)
    {
        hashed = (hashed * prime + string[i]) % num_buckets;
    }

    return (int)hashed;
}

int get_hash(const char *key, const int num_buckets, const int retries)
{
    const int hashed_a = hash(key, 151, num_buckets);
    const int hashed_b = hash(key, 163, num_buckets);
    return (hashed_a + (retries * (hashed_b + 1))) % num_buckets;
}

// Generic hash for strings (wraps get_hash)
unsigned long hash_string(void *key, int num_buckets, int retries)
{
    const char *str = (const char *)key;
    return (unsigned long)get_hash(str, num_buckets, retries);
}

// Hash for integers
unsigned long hash_int(void *key, int num_buckets, int retries)
{
    int *val = (int *)key;
    unsigned long h1 = (unsigned long)(*val) % num_buckets;
    unsigned long h2 = 1; // Secondary hash for probing
    return (h1 + (retries * (h2 + 1))) % num_buckets;
}

// Comparison for strings
int compare_string(void *a, void *b)
{
    const char *str_a = (const char *)a;
    const char *str_b = (const char *)b;
    return strcmp(str_a, str_b);
}

// Comparison for integers
int compare_int(void *a, void *b)
{
    int *int_a = (int *)a;
    int *int_b = (int *)b;
    if (*int_a < *int_b) return -1;
    if (*int_a > *int_b) return 1;
    return 0;
}

static bool is_prime(const int x)
{
    // Handle edge cases: 0 and 1 are not prime, 2 and 3 are prime
    if (x < 2) {
        return false;
    }
    if (x == 2 || x == 3) {
        return true;
    }
    // Even numbers > 2 are not prime
    if ((x % 2) == 0)
    {
        return false;
    }
    // Check divisibility by odd numbers up to sqrt(x)
    for (int i = 3; i <= floor(sqrt((double)x)); i += 2)
    {
        if ((x % i) == 0)
        {
            return false;
        }
    }
    return true;
}

int next_prime(int x)
{
    while (is_prime(x) != 1)
    {
        x++;
    }
    return x;
}
