/* $Id: examples.h 287 2004-07-30 16:28:21Z kpoitschke $ */
#ifndef __EXAMPLES_H
#define __EXAMPLES_H
#include "sqlora.h"

#undef __P
#if (defined(PROTOTYPES) || defined(__STDC__) || defined(__cplusplus) )
# define __P(protos) protos
#else
# define __P(protos) ()
#endif

/**
 * Print last sql error, rollback, logout and exit with FAILURE
 *
 * See error_exit.c
 */
void do_error_exit __P((sqlo_db_handle_t dbh, CONST char *file, int line,  
                        CONST char * msg));

#define error_exit(_dbh, _msg) do_error_exit(_dbh, __FILE__, __LINE__, _msg)

/**
 * ex1.c
 */
int table_exists __P((sqlo_db_handle_t dbh, char * table_name));

/**
 * ex2.c
 */
int col_count __P((sqlo_db_handle_t dbh, char * table_name));

/**
 * ex3.c
 */
int update_manager __P((sqlo_db_handle_t dbh));

/**
 * ex4.c
 */
sqlo_stmt_handle_t open_cursor __P((sqlo_db_handle_t dbh));

/**
 * ex5.c
 */
sqlo_stmt_handle_t open_cursor2 __P((sqlo_db_handle_t dbh, double min_income));

/**
 * ex6.c
 */
sqlo_stmt_handle_t reopen_cursor __P((sqlo_db_handle_t dbh, double min_income));

/**
 * ex7.c
 */
int do_select __P((sqlo_db_handle_t dbh, double min_income));

/**
 * ex8.c
 */
int update_emp __P((sqlo_db_handle_t dbh, double factor, const char * job));

/**
 * ex9.c
 */
int call_plsql __P((sqlo_db_handle_t dbh));

/**
 * ex10.c
 */
sqlo_stmt_handle_t prepare_cursor __P((sqlo_db_handle_t dbh, double* min_salary));

/**
 * ex11.c
 */
sqlo_stmt_handle_t do_select2 __P((sqlo_db_handle_t dbh, double min_salary));

/**
 * ex12.c
 */
int do_array_select __P((sqlo_db_handle_t dbh, double min_salary));

/**
 * ex13.c
 */
enum {
  MAX_BLOB_BUFFER_DATA = 65535
};

int insert_into_blob_table __P(( sqlo_db_handle_t dbh, int key ));
void create_blob_table __P((sqlo_db_handle_t dbh));
void drop_blob_table __P((sqlo_db_handle_t dbh));
void fillbuf __P((char * data, int len));
void fillbuf2 __P((char * data, int len));

/**
 * ex14.c
 */
int insert_file_into_blob_table __P((sqlo_db_handle_t dbh, int key, const char * fname ));

/**
 * ex15.c
 */
int select_from_blob_table __P(( sqlo_db_handle_t dbh, int key ));

/**
 * ex16.c
 */
int select_file_from_blob_table __P(( sqlo_db_handle_t dbh, int key, const char * fname ));

/**
 * ex17.c
 */
int select_refcursor __P((sqlo_db_handle_t dbh, double min_salary));

/**
 * ex18.c
 */
int select_refcursor2 __P((sqlo_db_handle_t dbh, double min_salary));

/**
 * ex19.c
 */
int select_ntable __P((sqlo_db_handle_t dbh));

/**
 * ex20.c
 */
int ex20 __P((sqlo_db_handle_t dbh, double min_salary));


#endif 
/* $Id: examples.h 287 2004-07-30 16:28:21Z kpoitschke $ */
