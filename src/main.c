#include <stdio.h>

#include "hash_table.c"
#include "utils.c"

int main() {
  printf("%d\n", hash("cat", 151, 53));
  printf("%d\n", hash("cat", 163, 53));
  return 0;
}