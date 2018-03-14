#include <random.h>
#include "tests/lib.h"
#include "tests/main.h"

int
main (int argc UNUSED, char *argv[]) 
{
  printf("main");
  test_name = argv[0];
  printf("test_name= argv");
  msg ("begin");
  printf("should have said begin ");
  random_init (0);
  test_main ();
  msg ("end");
  return 0;
}
