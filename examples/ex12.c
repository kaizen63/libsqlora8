/* $Id: ex12.c 221 2002-08-24 12:54:47Z kpoitschke $ */
#include <stdio.h>
#include <stdlib.h>
#include "examples.h"

enum {
  MAX_NAME_LEN = 64,
  MAX_ARRAY_SIZE = 4            /* Note: usually this should be 100 or so. 
                                 * Used a smaller one to do test all paths in the code
                                 */
};

/**
 * Structure to store the query results
 */
typedef struct _result_t {
  char name[MAX_NAME_LEN+1];    /* output ENAME */
  double salary;                /* output SAL */
  short name_ind;               /* indicator variable of ENAME */
  short sal_ind;                /* indicator variable of SAL */
  unsigned short name_rlen;     /* the actual length of ENAME */
  unsigned short name_rcode;    /* the return code of ENAME */
} result_t;

void print_record __P((result_t * r));


int do_array_select(sqlo_db_handle_t dbh, double min_salary)
{
  sqlo_stmt_handle_t sth;       /* statement handle */
  int i;                        /* loop variable */
  int status;                   /* return code of sqlo_... */
  result_t result[MAX_ARRAY_SIZE];
  int rows_fetched;             /* number of rows fetched per execute */
  int rows_fetched_total = 0;   /* total number of rows */
  int done_fetching = 0;        /* flag indicating end of fetch */
  double salary = min_salary;   /* input variable for SAL */
  int skip_size = sizeof(result[0]); /* The skip size */

  sth = prepare_cursor(dbh, &salary); /* see ex10.c */

  /* define output */
  if (SQLO_SUCCESS != 
      (sqlo_define_by_pos2(sth, 1, SQLOT_STR, result[0].name, sizeof(result[0].name), 
                           &result[0].name_ind, &result[0].name_rlen, 
                           &result[0].name_rcode, skip_size)) ||
      (sqlo_define_by_pos2(sth, 2, SQLOT_FLT, &result[0].salary, sizeof(result[0].salary),
                           &result[0].sal_ind, 0, 0, skip_size))) {
    error_exit(dbh, "sqlo_define_by_pos");
  }

  /* execute and fetch  the result */
  status = sqlo_execute(sth, 0);

  while (!done_fetching) {

    rows_fetched = MAX_ARRAY_SIZE;

    /* get the next set */
    status = sqlo_fetch(sth, MAX_ARRAY_SIZE);

    if (0 > status)
      error_exit(dbh, "sqlo_execute(NEXT)");
    else if (SQLO_NO_DATA == status) {
      
      rows_fetched = sqlo_prows(sth);
          
      /* sqlo_prows returns now the total number of fetched rows
       * the difference to the previous total fechted rows is
       * the number of rows fetched in this last call to sqlo_execute
       */
      rows_fetched = rows_fetched - rows_fetched_total;
      done_fetching = 1;

    }
    
    /* print the records */
    for (i = 0; i < rows_fetched; ++i)
      print_record(&result[i]);

    rows_fetched_total += rows_fetched;

  }
  printf("Selected %d employees\n", rows_fetched_total);
  /* finished. */
  sqlo_close(sth);

  return 1;
}

/* print record */
void print_record(result_t * r /* I - The record */ )
{
  printf("Name=%-8s Salary= %6.2f\n",
         (r->name_ind == SQLO_NULL_IND ? "NULL" : r->name),
         (r->sal_ind == SQLO_NULL_IND ? -1.0 : r->salary));
}

/* $Id: ex12.c 221 2002-08-24 12:54:47Z kpoitschke $ */
