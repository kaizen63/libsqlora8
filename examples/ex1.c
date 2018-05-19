/* $Id: ex1.c 221 2002-08-24 12:54:47Z kpoitschke $ */
#include <stdio.h>
#include <stdlib.h>
#include "examples.h"

int table_exists(sqlo_db_handle_t dbh, char * table_name)
{
  int stat;
  if ( 0 > (stat = sqlo_exists(dbh, "USER_TABLES", "TABLE_NAME", table_name, NULL))) {
    error_exit(dbh, "sqlo_exists");
   } 
  return stat == SQLO_SUCCESS ? 1 : 0;
 }

/* $Id: ex1.c 221 2002-08-24 12:54:47Z kpoitschke $ */
