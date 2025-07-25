#include <stdio.h>

#include "hash_table.c"
#include "utils.c"

int main() {
  printf("%d\n", get_key_hash("cat", 53, 0));
  printf("%d\n", get_key_hash("cat", 53, 1));
  return 0;
}