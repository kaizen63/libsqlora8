/* $Id: ex11.c 221 2002-08-24 12:54:47Z kpoitschke $ */
#include <stdio.h>
#include <stdlib.h>
#include "examples.h"

sqlo_stmt_handle_t do_select2(sqlo_db_handle_t dbh, double min_salary)
{
  sqlo_stmt_handle_t sth;       /* statement handle */
  int status;                   /* return code of sqlo_... */
  char name[64];                /* output variable for NAME */
  short name_rlen;              /* returned length of NAME */
  short name_ind;               /* NULL indicator for NAME */
  short sal_ind;                /* NULL indicator for SALARY */
  double salary = min_salary;   /* input variable for SALARY */

  sth = prepare_cursor(dbh, &salary); /* see ex10.c */

  /* define output */
  if (SQLO_SUCCESS != 
      (sqlo_define_by_pos(sth, 1, SQLOT_STR, name, sizeof(name), &name_ind, &name_rlen, 0)) ||
      (sqlo_define_by_pos(sth, 2, SQLOT_FLT, &salary, sizeof(salary), &sal_ind, 0, 0))) {
    error_exit(dbh, "sqlo_define_by_pos");
  }

  status = sqlo_execute(sth, 0);
  if ( 0 > status)
    error_exit(dbh, "sqlo_execute");

  while ( SQLO_SUCCESS == (status = sqlo_fetch(sth, 1))) {
    printf("Name=%-8s Salary=%6.2f\n", name, salary);
  }

  if (status != SQLO_NO_DATA)
    error_exit(dbh, "sqlo_fetch");

  return sth;
}
/* $Id: ex11.c 221 2002-08-24 12:54:47Z kpoitschke $ */
