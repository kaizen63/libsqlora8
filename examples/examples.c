/* $Id: examples.c 287 2004-07-30 16:28:21Z kpoitschke $ */
#include <stdio.h>
#include <stdlib.h>
#include "sqlora.h"
#include "examples.h"

static CONST char * _defuser = "scott/tiger";
static int _abort_flag = 0;

void sigint_handler(void);

void sigint_handler(void) {
  printf("Catched SIGINT\n");
  _abort_flag++;
}

#define RETURN_ON_ABORT if (_abort_flag) { sqlo_rollback(dbh); return EXIT_FAILURE; }

int main(int argc, char **argv)
{
  CONST char * cstr = _defuser;
  sqlo_db_handle_t dbh;
  int stat;
  sqlo_stmt_handle_t sth;
  double min_income;
  char server_version[1024];
  int handle;                   /* handle of the interrupt handler */

  if (argc > 1)
    cstr = argv[1];
  /* init */
  if (SQLO_SUCCESS != sqlo_init(SQLO_OFF, 1, 100)) {
    printf("Failed to init libsqlora8\n");
    return EXIT_FAILURE;
  }

  /* register the interrupt handler */
  sqlo_register_int_handler(&handle, sigint_handler);

  /* login */
  if (SQLO_SUCCESS != sqlo_connect(&dbh, cstr)) {
    printf("Cannot login with %s\n", cstr);
    return EXIT_FAILURE;
  }


  RETURN_ON_ABORT; /* finish if SIGINT was catched */

  if (SQLO_SUCCESS != sqlo_server_version(dbh, server_version, sizeof(server_version))) {
    printf("Failed to get the server version: %s\n", sqlo_geterror(dbh));
    return EXIT_FAILURE;
  }
  printf("Connected to:\n%s\n\n", server_version);

  RETURN_ON_ABORT; /* finish if SIGINT was catched */
  /* ex1.c */
  if (!table_exists(dbh, "EMP")) {
    printf("Table EMP does not exist. Please install Oracle demo tables\n");
    return EXIT_FAILURE;
  }

  RETURN_ON_ABORT; /* finish if SIGINT was catched */

  /* ex2.c */
  /* EMP should have 8 columns */
  if ( 8 != (stat = col_count(dbh, "EMP")))
    printf("Expected 8 columns in EMP, but counted %d\n", stat);

  RETURN_ON_ABORT; /* finish if SIGINT was catched */

  /* ex3.c */
  stat = update_manager(dbh);
  printf("Reduced the salary of %d managers\n", stat);

  sqlo_rollback(dbh);

  RETURN_ON_ABORT; /* finish if SIGINT was catched */

  /* ex4.c */
  sth = open_cursor(dbh);
  sqlo_close(sth);

  RETURN_ON_ABORT; /* finish if SIGINT was catched */

  /* ex5.c */
  sth = open_cursor2(dbh, 2500.0);
  sqlo_close(sth);

  RETURN_ON_ABORT; /* finish if SIGINT was catched */


  /* ex6.c */
  sth = reopen_cursor(dbh, 2500.0);
  if (sth != (stat = reopen_cursor(dbh, 5000.0))) {
    printf("reopen_cursor returned a new handle %d, expected %d\n", stat, sth);
  }
  /* we don't close this cursor */

  RETURN_ON_ABORT; /* finish if SIGINT was catched */

  /* ex7.c */
  do_select(dbh, 1000); /* note: we reuse the cursor opened by ex6.c */


  RETURN_ON_ABORT; /* finish if SIGINT was catched */

  /* ex8.c */
  stat = update_emp(dbh, 2.0, "MANAGER");
  printf("Doubled the salary of %d managers.\n", stat);

  sqlo_rollback(dbh);

  RETURN_ON_ABORT; /* finish if SIGINT was catched */

  /* ex9.c */
  call_plsql(dbh);


  RETURN_ON_ABORT; /* finish if SIGINT was catched */

  /* ex10.c */
  min_income = 2500;
  sth = prepare_cursor(dbh, &min_income);
  sqlo_close(sth);

  RETURN_ON_ABORT; /* finish if SIGINT was catched */

  /* ex11.c */
  printf("Employees with salary >= 3000:\n");
  do_select2(dbh, 3000); /* note: we reuse the cursor opened by ex6.c */
  
  RETURN_ON_ABORT; /* finish if SIGINT was catched */

  /* ex12.c */
  printf("Employees with salary >= 1200:\n");
  do_array_select(dbh, 1200);


  RETURN_ON_ABORT; /* finish if SIGINT was catched */

  /* ex13.c */
  insert_into_blob_table(dbh, 1);

  RETURN_ON_ABORT; /* finish if SIGINT was catched */

  /* ex13b.c */
  update_blob_table(dbh, 1);

  RETURN_ON_ABORT; /* finish if SIGINT was catched */

  /* ex14.c */
  insert_file_into_blob_table(dbh, 2, "examples");

  RETURN_ON_ABORT; /* finish if SIGINT was catched */

  /* ex15.c */
  select_from_blob_table(dbh, 1);

  RETURN_ON_ABORT; /* finish if SIGINT was catched */

  /* ex16.c */
  select_file_from_blob_table(dbh, 2, "examples.cmp");
  if ( 0 != system("diff -q examples examples.cmp")) {
    printf("ERROR: examples is different from examples.cmp!!!\n");
  }
  sqlo_rollback(dbh);

  RETURN_ON_ABORT; /* finish if SIGINT was catched */
  /* ex17.c */
  printf("Employees with salary (via refcursor) >= 1200:\n");
  select_refcursor(dbh, 1200);

  RETURN_ON_ABORT; /* finish if SIGINT was catched */
  /* ex18.c */
  printf("Employees with salary (via refcursor with bind vars) >= 1200:\n");
  select_refcursor2(dbh, 1200);

  RETURN_ON_ABORT; /* finish if SIGINT was catched */
  /* ex19.c */
  printf("Employees, departements and locations via ntable\n");
  select_ntable(dbh);


  /* ex20.c */
  printf("Example20:\n");

  ex20(dbh, 1000.0); /* note: we reuse the cursor opened by ex6.c*/

  /* rollback */
  sqlo_rollback(dbh);

  /* logout */
  sqlo_finish(dbh);

  return EXIT_SUCCESS;
}
/* $Id: examples.c 287 2004-07-30 16:28:21Z kpoitschke $ */
