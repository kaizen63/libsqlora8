/* $Id: ex3.c 221 2002-08-24 12:54:47Z kpoitschke $ */
#include <stdio.h>
#include <stdlib.h>
#include "examples.h"

int update_manager(sqlo_db_handle_t dbh)
{
  const char * argv[2];
  int stat;

  argv[0] = "0.5";
  argv[1] = "MANAGER";

  stat = sqlo_run(dbh, "UPDATE EMP SET SAL = SAL * :1 WHERE JOB = :2",
                 2, argv);
  if (0 > stat) {
    error_exit(dbh, "sqlo_run");
  }
  return stat;
}
/* $Id: ex3.c 221 2002-08-24 12:54:47Z kpoitschke $ */

