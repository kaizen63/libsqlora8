/* $Id: ex4.c 221 2002-08-24 12:54:47Z kpoitschke $ */
#include <stdio.h>
#include <stdlib.h>
#include "examples.h"

sqlo_stmt_handle_t open_cursor(sqlo_db_handle_t dbh)
{
  sqlo_stmt_handle_t sth;
  int argc = 0;
  const char * argv[1];
  
  argv[argc++] = "1000";

  if ( 0 > (sth = (sqlo_open(dbh, "SELECT ENAME, SAL FROM EMP WHERE SAL >= :1", 
                             argc, argv)))) {
    error_exit(dbh, "sqlo_open");
  }
  return sth;
}

/* $Id: ex4.c 221 2002-08-24 12:54:47Z kpoitschke $ */

