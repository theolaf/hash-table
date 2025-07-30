#pragma once

int get_hash(const char *key, const int num_buckets, const int retries);

int next_prime(int x);