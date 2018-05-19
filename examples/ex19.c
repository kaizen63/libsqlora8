/* $Id: ex19.c 221 2002-08-24 12:54:47Z kpoitschke $ */
#include <stdio.h>
#include <stdlib.h>
#include "examples.h"

int select_ntable(sqlo_db_handle_t dbh)
{
  sqlo_stmt_handle_t sth = SQLO_STH_INIT;
  sqlo_stmt_handle_t st2h;                     /* handle of the ref cursor */
  int status;
  char ename[11];
  char dname[15];
  char loc[14];
  short eind, dind, lind;
  int deptno = 10;

               /* don't know why the bind variable for deptno causes a crash */
  CONST char * stmt = 
    "SELECT ENAME, CURSOR(SELECT DNAME, LOC FROM DEPT)\n"
    "  FROM EMP WHERE DEPTNO = :deptno";


  /* parse the statement */
  if ( 0 <= (sth = sqlo_prepare(dbh, stmt))) {

    /* bind all variables */
    if (SQLO_SUCCESS != 
        (sqlo_bind_by_name(sth, ":deptno", SQLOT_INT, &deptno, sizeof(deptno), 0, 0) ) ) {
      error_exit(dbh, "sqlo_bind_by_name");
    } 

    /* Do the defines */
    /* You could also do: sqlo_define_by_pos(sth, 2, SQLOT_RSET, &st2h, 0, 0, 0, 0) */
    if (SQLO_SUCCESS != 
        ( sqlo_define_by_pos(sth, 1, SQLOT_STR, ename, sizeof(ename), &eind, 0, 0) ) ||
        ( sqlo_define_ntable(sth, 2, &st2h)))
      error_exit(dbh, "sqlo_define_ntable");
    
    /* execute the main statement */
    if (SQLO_SUCCESS != sqlo_execute(sth, 1))
      error_exit(dbh, "sqlo_execute");
    

    /* fetch from the main statement */
    while (SQLO_SUCCESS == (status = sqlo_fetch(sth, 1))) {
      printf("ENAME=%11s\n", ename);


      /* define the output of the second cursor */
      if (SQLO_SUCCESS != 
          ( sqlo_define_by_pos(st2h, 1, SQLOT_STR, dname, sizeof(dname), &dind, 0, 0) ) ||
          ( sqlo_define_by_pos(st2h, 2, SQLOT_STR, loc, sizeof(loc), &lind, 0, 0) ) )  {
          error_exit(dbh, "sqlo_define_by_pos");
        }

      /* execute the cursor */
      if(SQLO_SUCCESS != sqlo_execute(st2h, 1))
        error_exit(dbh, "sqlo_execute");

      /* fetch from the second cursor */
      while (SQLO_SUCCESS == (status = sqlo_fetch(st2h, 1))) {
        printf("    DNAME=%15s LOC=%15s\n", dname, loc);

      }
      if (status != SQLO_NO_DATA)
        error_exit(dbh, "sqlo_fetch(st2)");

    }
    
    if (SQLO_SUCCESS != sqlo_close(sth))
      error_exit(dbh, "sqlo_close(1)");
    
    if (SQLO_SUCCESS != sqlo_close(st2h))
      error_exit(dbh, "sqlo_close(2)");
    
  } else {
    error_exit(dbh, "sqlo_prepare");
  }

  return 1;
}

/* $Id: ex19.c 221 2002-08-24 12:54:47Z kpoitschke $ */
