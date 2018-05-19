/* $Id: ex9.c 221 2002-08-24 12:54:47Z kpoitschke $ */
#include <stdio.h>
#include <stdlib.h>
#include "examples.h"

static void create_sp __P((sqlo_db_handle_t dbh));

int call_plsql(sqlo_db_handle_t dbh)
{
  double ip1;
  int  ip2;
  char op[80];
  sqlo_stmt_handle_t sth = SQLO_STH_INIT;

  /* the stored procecdure call */
  char * stmt = 
    "BEGIN\n"
    "    EX9(:ip1, :ip2, :op);\n"
    "END;\n";


  create_sp(dbh);               /* create the test stored procedure */

  ip1 = 1.5;
  ip2 = 20;


  /* parse the statement */
  if ( 0 <= (sth = sqlo_prepare(dbh, stmt))) {
    /* bind all variables */
    if (SQLO_SUCCESS != 
        (sqlo_bind_by_name(sth, ":ip1", SQLOT_FLT, &ip1, sizeof(ip1), 0, 0) ||
         sqlo_bind_by_name(sth, ":ip2", SQLOT_INT, &ip2, sizeof(ip2), 0, 0) ||
         sqlo_bind_by_name(sth, ":op", SQLOT_STR, &op, sizeof(op), 0, 0)
         )) {

      error_exit(dbh, "sqlo_bind_by_name");
    } else {
      /* execute the call */
      if (SQLO_SUCCESS != sqlo_execute(sth, 1))
        error_exit(dbh, "sqlo_execute");
    }

    /* print the result */
    if (atof(op) != (ip1 + ip2))
      printf("Stored procudure returned wrong result %s, expected %6.2f\n",
             op, (ip1 + ip2));

    if (SQLO_SUCCESS != sqlo_close(sth))
      error_exit(dbh, "sqlo_execute");

  } else {
    error_exit(dbh, "sqlo_prepare");
  }

  if ( 0 > sqlo_exec(dbh, "DROP PROCEDURE EX9"))
    error_exit(dbh, "sqlo_exec");

  return 1;
}

/* creates the stored procedure used above */
static void
create_sp(sqlo_db_handle_t dbh)
{
  char * stmt =
    "CREATE OR REPLACE PROCEDURE EX9(ipNum1 IN NUMBER, ipNum2 IN NUMBER, opStr OUT VARCHAR)\n"
    "IS\n"
    "BEGIN\n"
    "  opStr := TO_CHAR(ipNum1 + ipNum2);\n"
    "END;\n";

  if (SQLO_SUCCESS != sqlo_exec(dbh, stmt))
    error_exit(dbh, stmt);

}

/* $Id: ex9.c 221 2002-08-24 12:54:47Z kpoitschke $ */
