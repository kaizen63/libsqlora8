/* $Id: ex10.c 221 2002-08-24 12:54:47Z kpoitschke $ */
#include <stdio.h>
#include <stdlib.h>
#include "examples.h"

sqlo_stmt_handle_t prepare_cursor(sqlo_db_handle_t dbh, double * min_salary)
{
  sqlo_stmt_handle_t sth;                      /* statement handle */

  if (0 > (sth = sqlo_prepare(dbh, 
                            "SELECT ENAME, SAL FROM EMP WHERE SAL >= :salary ORDER BY 2,1")))
    error_exit(dbh, "sqlo_prepare");

  if (SQLO_SUCCESS != 
      sqlo_bind_by_pos(sth, 1, SQLOT_FLT, min_salary, sizeof(double), 0, 0))
    error_exit(dbh, "sqlo_bind_by_pos");

  return sth;
}
/* $Id: ex10.c 221 2002-08-24 12:54:47Z kpoitschke $ */
