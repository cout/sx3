#include <stdio.h>
#include "c_itor.h"
#include "c_algo.h"

void do_print(ARRAY_IT(int) it) {
  printf("%d\n", ARRAY_IT_GET(it));
}

int main() {
  int a[3] = {10, 20, 30};
  FOR_EACH(ARRAY, int, STACK_ARRAY_BEGIN(a), STACK_ARRAY_END(a), do_print(it));

  return 0;
}
