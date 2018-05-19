/* $Id: ex20.c 301 2005-01-08 17:28:49Z kpoitschke $ */
#include <stdio.h>
#include <stdlib.h>
#include "examples.h"

/* Select employees with salary > min_income using sqlo_query_result */

int ex20(sqlo_db_handle_t dbh, double min_income)
{
  sqlo_stmt_handle_t sth;       /* statement handle */
  int status;                   /* status of sqlo calls */
  unsigned int i,j;                      /* loop counter */
  char ** v;                    /* values */
  char ** col_names;           /* column names */
  unsigned int *nl;            /* column name lengths */
  unsigned short *vl;           /* value lengths */
  unsigned int nc;              /* number of columns */
  int nrows;

  sth = reopen_cursor(dbh, min_income); /* see example of sqlo_reopen (ex6.c) */


  printf("Employees with SAL > %-8.2f:\n", min_income);


  nrows = 0;
  /* fetch the data */
  while ( SQLO_SUCCESS == (status = (sqlo_query_result(sth, /* statement */
						       &nc, /* number of columns */
						       &v,  /* values */
						       &vl, /* value lengths */
						       &col_names, /* column names */
						       &nl   /* column name lengths */
						       )))) {
    
    if ( nrows == 0 )
      {
	/* print the header */
	for (i = 0; i < nc; ++i)
	  printf("%-*s ", nl[i]+4, col_names[i]);
	printf("\n");
	for (i = 0; i < nc; ++i) {
	  for (j = 0; j < nl[i]+4; ++j) {
	    putchar('-');
	  }
	  putchar('+');
	}
	putchar('\n');
      }
    
    /* print the column values */
    for (i = 0; i < nc; ++i)
      printf("%-*s ", (vl[i] > nl[i] ? vl[i] : nl[i])+4, v[i]);
    
    printf("\n");

    ++nrows;
  }

  if (0 > status) {
    error_exit(dbh, "sqlo_query_result");
  }

  if ( SQLO_SUCCESS != sqlo_close(sth))
    error_exit(dbh, "sqlo_close");

  return 1;

}

/* $Id: ex20.c 301 2005-01-08 17:28:49Z kpoitschke $ */
   
