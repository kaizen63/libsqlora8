/* $Id: ex8.c 221 2002-08-24 12:54:47Z kpoitschke $ */
#include <stdio.h>
#include <stdlib.h>
#include "examples.h"

int update_emp(sqlo_db_handle_t dbh, double factor, const char * job)
{
  int stat;
  char stmt[1024];

  sprintf(stmt, "UPDATE EMP SET SAL = SAL * %f WHERE JOB = '%s'",
          factor, job);

  if ( 0 > (stat = sqlo_exec(dbh, stmt)))
    error_exit(dbh, "sqlo_run");

  return stat;                  /* number of processed rows */
}

/* $Id: ex8.c 221 2002-08-24 12:54:47Z kpoitschke $ */

