/* $Id: ex17.c 221 2002-08-24 12:54:47Z kpoitschke $ */
#include <stdio.h>
#include <stdlib.h>
#include "examples.h"

int select_refcursor(sqlo_db_handle_t dbh, double min_salary)
{
  sqlo_stmt_handle_t sth = SQLO_STH_INIT;
  sqlo_stmt_handle_t st2h;                     /* handle of the ref cursor */
  double sal = min_salary;
  CONST char **v;
  int status;

  CONST char * stmt = 
    "BEGIN\n"
    "    OPEN :c1 FOR SELECT ENAME, SAL FROM EMP WHERE SAL >= :min_sal ORDER BY 2,1;\n"
    "END;\n";


  /* parse the statement */
  if ( 0 <= (sth = sqlo_prepare(dbh, stmt))) {
    /* bind all variables */
    if (SQLO_SUCCESS != 
        ( sqlo_bind_ref_cursor(sth, ":c1", &st2h)) ||
        (sqlo_bind_by_name(sth, ":min_sal", SQLOT_FLT, &sal, sizeof(sal), 0, 0) ) ) {
      error_exit(dbh, "sqlo_bind_by_name");
    } else {

      /* execute the PL/SQL block */
      if (SQLO_SUCCESS != sqlo_execute(sth, 1))
        error_exit(dbh, "sqlo_execute");
    }
    /* execute the refcursor */
    if (SQLO_SUCCESS != sqlo_execute(st2h, 1)) {
      error_exit(dbh, "sqlo_execute(ref)");
    }

    while (SQLO_SUCCESS == (status = sqlo_fetch(st2h, 1))) {
      v = sqlo_values(st2h, NULL, 1);

      printf("Name=%-8s Salary= %-6s\n", v[0], v[1]);
    }
    
    if (status != SQLO_NO_DATA)
      error_exit(dbh, "sqlo_fetch(st2)");

    if (SQLO_SUCCESS != sqlo_close(sth))
      error_exit(dbh, "sqlo_close(1)");

    if (SQLO_SUCCESS != sqlo_close(st2h))
      error_exit(dbh, "sqlo_close(2)");

  } else {
    error_exit(dbh, "sqlo_prepare");
  }

  return 1;
}

/* $Id: ex17.c 221 2002-08-24 12:54:47Z kpoitschke $ */
