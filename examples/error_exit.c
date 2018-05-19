#include <stdio.h>
#include <stdlib.h>
#include "examples.h"

/* print error and exit with failure */
void do_error_exit(sqlo_db_handle_t dbh, const char * file, int line, const char * msg)
{
  printf("%s (line: %d):\n%s: %s\n", 
         file,
         line,
         msg, 
         sqlo_geterror(dbh));
  sqlo_rollback(dbh);
  sqlo_finish(dbh);
  exit(EXIT_FAILURE);
}
