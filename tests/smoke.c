#include <opentracedecode/opentracedecode.h>
#include <stdio.h>

int main(void) {
  if (otd_init(NULL) != 0) return 1;
  puts("otd ok");
  otd_exit();
  return 0;
}
