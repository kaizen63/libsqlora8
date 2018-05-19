/* $Id: ex7.c 287 2004-07-30 16:28:21Z kpoitschke $ */
#include <stdio.h>
#include <stdlib.h>
#include "examples.h"

int do_select(sqlo_db_handle_t dbh, double min_income)
{
  sqlo_stmt_handle_t sth;       /* statement handle */
  int status;                   /* status of sqlo calls */
  unsigned int i,j;                      /* loop counter */
  const char ** v;              /* values */
  const char ** col_names;      /* column names */
  CONST unsigned int *nl;       /* column name lengths */
  CONST unsigned short *vl;     /* value lengths */
  unsigned int nc;              /* number of columns */


  sth = reopen_cursor(dbh, min_income); /* see example of sqlo_reopen (ex6.c) */

  /* get the output column names */
  status = sqlo_ocol_names2(sth, &nc, &col_names);

  if (0 > status) {
    error_exit(dbh, "sqlo_ocol_names2");
  }

  /* get the output column name lengths */
  nl = sqlo_ocol_name_lens(sth, NULL);

  printf("Employees with SAL > %-8.2f:\n", min_income);

  /* print the header */
  for (i = 0; i < nc; ++i)
    printf("%-*s ", nl[i], col_names[i]);

  printf("\n");
  for (i = 0; i < nc; ++i) {
    for (j = 0; j < nl[i]; ++j) {
      putchar('-');
    }
    putchar('+');
  }
  putchar('\n');

  /* fetch the data */
  while ( SQLO_SUCCESS == (status = (sqlo_fetch(sth, 1)))) {
    
    /* get one record */
    v = sqlo_values(sth, NULL, 1);

    /* get the length of the data items */
    vl = sqlo_value_lens(sth, NULL);

    /* print the column values */
    for (i = 0; i < nc; ++i)
      printf("%-*s ", (vl[i] > nl[i] ? vl[i] : nl[i]), v[i]);

    printf("\n");

  }

  if (0 > status) {
    error_exit(dbh, "sqlo_fetch");
  }

  /*
  if ( SQLO_SUCCESS != sqlo_close(sth))
    error_exit(dbh, "sqlo_close");
  */
  return 1;

}

/* $Id: ex7.c 287 2004-07-30 16:28:21Z kpoitschke $ */
