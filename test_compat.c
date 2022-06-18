/*BINFMTC: compat.c
  test compat.c
 */

#define __TEST_COMPAT_C__
#include <assert.h>
#include <stdio.h>
#include "compat.c"

static int test_asprintf(void) {
  char* s = NULL;

  asprintf(&s, "test code %s\n", "test");
  assert(!strcmp(s, "test code test\n"));
  return 0;
}

int main() {
  test_asprintf();
  return 0;
}
