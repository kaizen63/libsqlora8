/* $Id: ex5.c 221 2002-08-24 12:54:47Z kpoitschke $ */
#include <stdio.h>
#include <stdlib.h>
#include "examples.h"

sqlo_stmt_handle_t open_cursor2(sqlo_db_handle_t dbh, double min_income)
{
  sqlo_stmt_handle_t sth = SQLO_STH_INIT;
  int argc = 0;
  const char * argv[1];
  char hlp[64];

  sprintf(hlp, "%f", min_income);

  argv[argc++] = hlp;

  if ( 0 > (sqlo_open2(&sth, dbh, 
                       "SELECT ENAME, SAL FROM EMP WHERE SAL >= :1", 
                       argc, argv))) {
    error_exit(dbh, "sqlo_open");
  }

  return sth;
}
/* $Id: ex5.c 221 2002-08-24 12:54:47Z kpoitschke $ */
