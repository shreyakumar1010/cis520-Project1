/* Tests the halt system call. */

#include "tests/lib.h"
#include "tests/main.h"

void
test_main (void) 
{
  ASSERT(false);
  halt ();
  fail ("should have halted");
}
