#include <stdio.h>
#include "c_dynarray.h"
#include "c_algo.h"

void do_print(DYNARRAY_IT(int) it) {
  printf("%d\n", DYNARRAY_IT_GET(it));
}

int main() {
  DYNARRAY(int) da;
  DYNARRAY_INIT(da);

  DYNARRAY_PUSH_BACK_IMM(da, int, 10);
  DYNARRAY_PUSH_BACK_IMM(da, int, 20);
  DYNARRAY_PUSH_BACK_IMM(da, int, 30);

  FOR_EACH(DYNARRAY, int, DYNARRAY_BEGIN(da), DYNARRAY_END(da), do_print);

  DYNARRAY_FREE(da);

  return 0;
}
