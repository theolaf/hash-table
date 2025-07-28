#include <string.h>
#include <math.h>

int hash(const char* string, const int prime, const int num_buckets) {
    long hashed = 0;
    const int string_length = strlen(string);

    for (int i = 0; i < string_length; i++) {
        hashed += (long)pow(prime, string_length - (i+1)) * string[i];
    }

    return (int)hashed % num_buckets;
}