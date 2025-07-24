#include <stdio.h>

#include "utils.h"

int main() {
  printf("%d\n", hash("cat", 151, 53));
  printf("%d\n", hash("cat", 163, 53));
  return 0;
}