/* $Id: ex2.c 221 2002-08-24 12:54:47Z kpoitschke $ */
#include <stdio.h>
#include <stdlib.h>
#include "examples.h"

int col_count(sqlo_db_handle_t dbh, char * table_name)
{
  int stat;
  if ( 0 > (stat = sqlo_count(dbh, "USER_TAB_COLUMNS", "TABLE_NAME", table_name, NULL))) {
    error_exit(dbh, "sqlo_count");
  }
  return stat;
}

/* $Id: ex2.c 221 2002-08-24 12:54:47Z kpoitschke $ */
