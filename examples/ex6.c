/* $Id: ex6.c 221 2002-08-24 12:54:47Z kpoitschke $ */
#include <stdio.h>
#include <stdlib.h>
#include "examples.h"

sqlo_stmt_handle_t reopen_cursor(sqlo_db_handle_t dbh, double min_income)
{
  static sqlo_stmt_handle_t sth = SQLO_STH_INIT;
  int argc = 0;
  const char * argv[1];
  char hlp[64];

  sprintf(hlp, "%f", min_income);

  argv[argc++] = hlp;

  if (SQLO_STH_INIT == sth) {
    /* first time, open a new cursor */
    if ( 0 > (sqlo_open2(&sth, dbh, 
                         "SELECT ENAME, SAL FROM EMP WHERE SAL >= :1", 
                         argc, argv))) {
      error_exit(dbh, "sqlo_open");
    }
  } else {
    /* next time, bind again with new variables */
    if ( 0 > sqlo_reopen(sth, argc, argv))
      error_exit(dbh, "sqlo_reopen");
  }

  return sth;
}
/* $Id: ex6.c 221 2002-08-24 12:54:47Z kpoitschke $ */
