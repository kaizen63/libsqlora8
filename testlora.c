/* $Id: testlora.c 337 2005-12-19 12:31:30Z kpoitschke $
 *
 * Copyright (c) 1991-2004 Kai Poitschke (kai@poitschke.de)
 *  
 * 
 * This file is part of the libsqlora8 package which can be found
 * at http://www.poitschke.de/libsqlora8/
 *
 *
 *  Permission to use, copy, modify, and distribute this software for
 *  any purpose with or without fee is hereby granted, provided that
 *  the above copyright notice and this permission notice appear in all
 *  copies.
 *
 *  THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 *  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL THE AUTHORS AND COPYRIGHT HOLDERS AND THEIR
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 *  USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 *  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 *  OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 *  SUCH DAMAGE.
 *
 * testlora.c 
 * Test programm for libsqlora8.a (Kai Poitschke)
 *-----------------------------------------------------------------------*/
#ifdef HAVE_CONFIG_H
#include "config.h"             /* read config to get ENABLE_PTHREADS */
#endif

#include <stdio.h>
#ifndef _MSC_VER
#include <unistd.h>
#endif
#include <signal.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>

#ifdef _MSC_VER
#define sleep(a) _sleep(a)
#endif

#define TEST_NONBLOCKING 1

#if (defined(ENABLE_PTHREADS) && defined(HAVE_PTHREAD_H))
#  include <pthread.h>
#endif

#if defined(ENABLE_ORATHREADS)
#define ENABLE_PTHREADS
#endif

#include "sqlora.h"


#define MAX_ITERS 254

#define MAX_LOOPS 1           /* number of runs/threads */

#define CLOSE_CURSOR 1

#define MAX_ARRAYSIZE 100


static int g_dbh[MAX_LOOPS];
static char * g_cstr = NULL;
static int g_nonblocking_mode = 0;

#if defined(ENABLE_PTHREADS)  && defined(HAVE_PTHREAD_H)
static pthread_t g_thr[MAX_LOOPS];
static pthread_mutex_t g_create_tables_lock;
#endif


/* If we have usleep we wait this amount of microseconds in a 
 * OCI_STILL_EXECUTING loop
 */
#ifdef HAVE_USLEEP
#  define SQLO_USLEEP usleep(20000)
#else
#  define SQLO_USLEEP
#endif

#undef __P
#if (defined(PROTOTYPES) || defined(__STDC__) || defined(__cplusplus) )
# define __P(protos) protos
#else
# define __P(protos) ()
#endif

#define greatest(_A,_B) ( (_A) > (_B) ? (_A) : (_B) )

int test_array_insert __P(( int dbh, int thread_id, int delete_flag ));
void sig_handler __P((int signal));
int create_table __P((int dbh));
int create_table_long __P((int dbh));
int create_table_clob __P((int dbh));
int delete_table __P((int dbh, char *table_name, int thread_id));
int insert_into_long_table __P((int dbh, int thread_id));
int select_from_long_table __P((int, int thread_id));
int test_long __P((int dbh, int thread_id));
int insert_into_clob_table __P((int dbh, int thread_id));
int select_from_clob_table __P((int, int thread_id));
int test_clob __P((int dbh, int thread_id));
int do_select __P((int dbh, int thread_id));
int test_select2 __P((int dbh, int thread_id));
int test_reopen __P((int dbh, int thread_id));
int create_packages __P((int dbh));
int test_plsql __P((int dbh, int thread_id));
int test_insert __P(( int dbh, int thread_id, int delete_flag));
int test_array_insert __P((int dbh, int thread_id, int delete_flag));
int test_array_insert2 __P(( int dbh, int thread_id, int delete_flag ));
int test_exists __P((int dbh, int thread_id));
int test_count __P((int dbh, int thread_id));
int test_exec __P((int dbh, int thread_id));
int create_tables __P((int dbh));
int cleanup __P((int dbh));
int delete_table __P(( int dbh, char * table_name, int thread_id ));
int test_sqlo_query_result __P((int dbh, int thread_id));
int test_success_with_info __P((int, int thread_id));
int test_autocommit __P((int, int thread_id));

#if defined(ENABLE_PTHREADS)  && defined(HAVE_PTHREAD_H)
void * run_test __P((void * arg));
#else
int run_test __P((int arg));
#endif

static int start_test __P((void));
static int join_threads __P((void));

void sig_handler(int signal)
{
  abort();
}

/*-------------------------------------------------------------------------
 * create our test table
 *-----------------------------------------------------------------------*/
int create_table( int dbh )
{
  int status;

  char * create_table = 
    "CREATE TABLE T_SQLORA_TEST (\n"
    "THREAD_ID NUMBER     NOT NULL,\n"
    "NKEY NUMBER(8)       NULL,\n"
    "CKEY VARCHAR2(5)     NULL,\n"
    "NVAL NUMBER(16,4)    NULL,\n"
    "CVAL VARCHAR2(20)    NULL,\n"
    "FVAL FLOAT(126)      NULL,\n"
    "DVAL DATE)";

  printf("Create table T_SQLORA_TEST\n");

  /* Check if the table already exists */
  if (SQLO_NO_DATA == 
      sqlo_exists(dbh, "USER_TABLES", "TABLE_NAME", "T_SQLORA_TEST", NULL))
    {
      /* No, create it */
      while (SQLO_STILL_EXECUTING == (status = sqlo_exec(dbh, create_table)))
        {
          printf(".");
          SQLO_USLEEP;
        }
      printf("\n");

      if (SQLO_SUCCESS != status)
        {
          printf("create_table failed: %s\n%s\n", sqlo_geterror(dbh), 
                 create_table);
          return 0;
        }
      printf("Table T_SQLORA_TEST created\n");
    }

  return 1;
}

/*-------------------------------------------------------------------------
 * create  a test table with a long field
 *-----------------------------------------------------------------------*/
int create_table_long( int dbh )
{
  int status;

  char * create_table = 
    "CREATE TABLE T_SQLORA_TEST_LONG (\n"
    "THREAD_ID NUMBER     NOT NULL,\n"
    "DATA LONG)";

  printf("Create Table T_SQLORA_TEST_LONG\n");
  /* Check if the table already exists */
  if (SQLO_NO_DATA == 
      sqlo_exists(dbh, "USER_TABLES", "TABLE_NAME", "T_SQLORA_TEST_LONG", NULL))
    {
      /* No, create it */
      while (SQLO_STILL_EXECUTING == (status = sqlo_exec(dbh, create_table)))
        {
          printf(".");
          SQLO_USLEEP;
        }
      printf("\n");

      if (SQLO_SUCCESS != status)
        {
          printf("create_table failed: %s\n%s\n", sqlo_geterror(dbh), 
                 create_table);
          return 0;
        }
      printf("Table T_SQLORA_TEST_LONG created\n");
    }

  return 1;
}

/*-------------------------------------------------------------------------
 * create  a test table with a clob field
 *-----------------------------------------------------------------------*/
int create_table_clob( int dbh )
{
  int status;

  char * create_table = 
    "CREATE TABLE T_SQLORA_TEST_CLOB (\n"
    "THREAD_ID NUMBER     NOT NULL,\n"
    "DATA CLOB)";

  printf("Create Table T_SQLORA_TEST_CLOB\n");
  /* Check if the table already exists */
  if (SQLO_NO_DATA == 
      sqlo_exists(dbh, "USER_TABLES", "TABLE_NAME", "T_SQLORA_TEST_CLOB", NULL))
    {
      /* No, create it */
      while (SQLO_STILL_EXECUTING == (status = sqlo_exec(dbh, create_table)))
        {
          printf(".");
          SQLO_USLEEP;
        }
      printf("\n");

      if (SQLO_SUCCESS != status)
        {
          printf("create_table failed: %s\n%s\n", sqlo_geterror(dbh), 
                 create_table);
          return 0;
        }
      printf("Table T_SQLORA_TEST_CLOB created\n");
    }

  return 1;
}


/*-------------------------------------------------------------------------
 * delete the test table
 *-----------------------------------------------------------------------*/
int delete_table( int dbh, char * table_name, int thread_id )
{
  char sqlcmd[512];
  int status;

  sprintf(sqlcmd, "DELETE %s WHERE THREAD_ID=%d", table_name, thread_id);

  while ( SQLO_STILL_EXECUTING == (status = sqlo_exec(dbh, sqlcmd)))
    {
      printf(".");
      SQLO_USLEEP;
    }
  printf("\n");

  if (status < 0)
    {
      printf("delete failed: (status=%d) %s\n%s\n", 
             status, sqlo_geterror(dbh), 
             sqlcmd);
      return 0;
    }

  return 1;
}

/*-------------------------------------------------------------------------
 * Insert data into T_SQLORA_TEST_LONG 
 *-----------------------------------------------------------------------*/
int insert_into_long_table( int dbh, int thread_id )
{
#define MAX_DATA 65535
  char * stmt = 
    "INSERT INTO T_SQLORA_TEST_LONG (THREAD_ID, DATA) VALUES (:b1, :b2)";
  int i;
  int argc;
  char const * argv[3];
  char data[MAX_DATA+1];
  char sthread_id[16];

  printf("Insert data into long table\n");

  sprintf(sthread_id, "%d", thread_id);
  /* fill the data */
  for (i = 0; i < MAX_DATA; ++i)
    {
      data[i] = 'A' + i % 26;
    }

  data[i] = '\0';
  argc = 0;
  argv[argc++] = sthread_id;
  argv[argc++] = data;

  if ( 0 > sqlo_run(dbh, stmt, argc, argv))
    {
      printf("sqlo_run failed: %s\n", sqlo_geterror(0));
      return (0);
    }

  return (1);

}



/*-------------------------------------------------------------------------
 * Insert data into T_SQLORA_TEST_CLOB
 *-----------------------------------------------------------------------*/
int insert_into_clob_table( int dbh, int thread_id )
{
#define MAX_DATA 65535
  char * stmt = 
    "INSERT INTO T_SQLORA_TEST_CLOB (THREAD_ID, DATA) VALUES (:b1, empty_clob()) returning data into :b2";

  char data[MAX_DATA+1];
  sqlo_lob_desc_t loblp;
  sqlo_stmt_handle_t sth;
  int i;
  int status;

  printf("Insert data into clob table\n");


  /* fill the data */
  for (i = 0; i < MAX_DATA; ++i)
    {
      data[i] = 'A' + i % 26;
    }

  data[i] = '\0';

  if (0 > (sth = sqlo_prepare( dbh, stmt )) )
    {
      printf("insert_into_clob_table: sqlo_prepare failed: %s\n", 
	     sqlo_geterror( dbh ));
      return (0);
    }

  if (0 > sqlo_alloc_lob_desc( dbh, &loblp ) )
    {
      printf("insert_into_clob_table: sqlo_alloc_lob_desc failed: %s\n", 
	     sqlo_geterror( dbh ));
      return (0);
    }

  if (SQLO_SUCCESS != 
      sqlo_bind_by_pos( sth, 1, SQLOT_INT, &thread_id, sizeof(int), NULL, 0)
      )
    {
      printf("insert_into_clob_table: sqlo_bind_by_pos(1) failed: %s\n", 
	     sqlo_geterror( dbh ));
      sqlo_free_lob_desc(dbh, &loblp);
      return (0);
    }

  if (SQLO_SUCCESS != 
      sqlo_bind_by_pos( sth, 2, SQLOT_CLOB, &loblp, 0, NULL, 0)
      )
    {
      printf("insert_into_clob_table: sqlo_bind_by_pos(2) failed: %s\n", 
	     sqlo_geterror( dbh ));
      sqlo_free_lob_desc(dbh, &loblp);
      return (0);
    }

  while ( SQLO_STILL_EXECUTING == (status = sqlo_execute( sth, 1 ) ))
    {
      SQLO_USLEEP;
    }

  if (status < 0 )
    {
      sqlo_free_lob_desc(dbh, &loblp);
      printf("insert_into_clob_table: sqlo_execute failed: %s\n", 
	     sqlo_geterror( dbh ));

      return (0);
    }

  while (SQLO_STILL_EXECUTING == 
	 (status = sqlo_lob_write_buffer( dbh, loblp, MAX_DATA, data, 
					  MAX_DATA, SQLO_ONE_PIECE)))
    {
      SQLO_USLEEP;
    }
	 

  if (status < 0 )
    {
      sqlo_free_lob_desc(dbh, &loblp);
      printf("insert_into_clob_table: sqlo_lob_write_buffer failed (status=%d): %s\n", status, sqlo_geterror( dbh ));

      return (0);
    }

  sqlo_free_lob_desc(dbh, &loblp);
  sqlo_close( sth );

  printf("Insert data into clob table finished successfull\n");

  return (1);

}



/*-------------------------------------------------------------------------
 * Select data from T_SQLORA_TEST_LONG 
 *-----------------------------------------------------------------------*/
int select_from_long_table( int dbh, int thread_id )
{
  char * stmt = 
    "SELECT THREAD_ID, DATA FROM T_SQLORA_TEST_LONG";
  int status;
  int sth = SQLO_STH_INIT;
  char const **v;

  printf("Select data from long table\n");

  while ( SQLO_STILL_EXECUTING == (status= sqlo_open2(&sth, dbh, stmt, 0, NULL)))
    {
      SQLO_USLEEP;
    }

  if (status < 0)
    {
      printf("sqlo_open failed: %s\n", sqlo_geterror(0));
      return (0);
    }

  while (SQLO_SUCCESS == (status = sqlo_fetch(sth, 1)) || status == SQLO_STILL_EXECUTING)
    {
      if (status == SQLO_STILL_EXECUTING) 
        {
          SQLO_USLEEP;
          continue;
        }

      v = sqlo_values(sth, NULL, 0);
      printf("ThreadID: %s, Data: %.60s\n", v[0], v[1]);
    }

  if (status < 0) 
    {
      printf("ERROR during fetch: %s", sqlo_geterror(dbh));
      return 0;
    }
  sqlo_close(sth);

  if (!delete_table(dbh, "T_SQLORA_TEST_LONG", thread_id))
    return(0);

  return (1);

}

/*-------------------------------------------------------------------------
 * Select data from T_SQLORA_TEST_CLOB via std interface and handle
 * the failure
 *-----------------------------------------------------------------------*/
int select_from_clob_table( int dbh, int thread_id )
{
  char * stmt = 
    "SELECT THREAD_ID, DATA FROM T_SQLORA_TEST_CLOB";
  int status;
  int sth = SQLO_STH_INIT;
  char const **v;

  printf("Select data from clob table\n");
  printf("The next error -30012 is o.k. because we cannot query a CLOB via the easy interface\n");
  while ( SQLO_STILL_EXECUTING == (status= sqlo_open2(&sth, dbh, stmt, 0, NULL)))
    {
      SQLO_USLEEP;
    }

  /* We have to get an error here, because it is not allowed to query a lob via sqlo_open */

  if (status < 0)
    {
      if ( status == SQLO_UNSUPPORTED_DATA_TYPE ) 
	{
	  printf("sqlo_open2 detected the unsupported data type: %s\n", sqlo_geterror(dbh));
	  return (1);
	}
      else
	{
	  printf("sqlo_open2 failed unexpected with: %s\n", sqlo_geterror(dbh));
	  return (0);
	}
    }
  else
    {
      printf("sqlo_open2 failed to detect the CLOB query\n");
      return (0);
    }

  while (SQLO_SUCCESS == (status = sqlo_fetch(sth, 1)) || status == SQLO_STILL_EXECUTING)
    {
      if (status == SQLO_STILL_EXECUTING) 
        {
          SQLO_USLEEP;
          continue;
        }

      v = sqlo_values(sth, NULL, 0);
      printf("ThreadID: %s, Data: %.60s\n", v[0], v[1]);
    }

  if (status < 0) 
    {
      printf("ERROR during fetch: %s", sqlo_geterror(dbh));
      return 0;
    }
  sqlo_close(sth);

  if (!delete_table(dbh, "T_SQLORA_TEST_CLOB", thread_id))
    return(0);

  return (1);

}



/*-------------------------------------------------------------------------
 * test_long
 *-----------------------------------------------------------------------*/
int test_long ( int dbh, int thread_id )
{
  printf ("Test long\n");
  if (!create_table_long(dbh))
    return (0);

  if (!insert_into_long_table(dbh, thread_id))
    return (0);

  if (!select_from_long_table(dbh, thread_id))
    return (0);

  printf ("Test long ok\n");
  return (1);
}



/*-------------------------------------------------------------------------
 * test_clob
 *-----------------------------------------------------------------------*/
int test_clob ( int dbh, int thread_id )
{
  printf ("Test clob with std. interface\n");
  if (!create_table_clob(dbh))
    return (0);

  if (!insert_into_clob_table(dbh, thread_id))
    return (0);

  if (!select_from_clob_table(dbh, thread_id))
    return (0);

  printf ("Test long ok\n");
  return (1);
}



/*-------------------------------------------------------------------------
 * Query the test table
 *-----------------------------------------------------------------------*/
int do_select( int dbh, int thread_id )
{
  int sth = SQLO_STH_INIT;
  const char **v;
  int argc;
  const char *argv[1];
  const char **ocol_names;
  const int * ocol_name_lens;
  const unsigned short * vl;
  int nrows;
  char sthread_id[16];

  int n_ocols;
  int i;
  int status;

  char * select_stmt =
    "SELECT THREAD_ID THID, NKEY AS NUM_KEY, CKEY AS CHAR_KEY, TO_CHAR(NVAL,'9999999999.999') AS NVAL, CVAL, DVAL, FVAL FROM T_SQLORA_TEST WHERE THREAD_ID = :1";

  sprintf(sthread_id,"%d", thread_id);
  argc = 0;
  argv[argc++] = sthread_id;

  /* Select all and display */
  while (SQLO_STILL_EXECUTING == (status = sqlo_open2(&sth, dbh, select_stmt, argc, argv)))
    {
      printf(".");
      SQLO_USLEEP;
    }

  printf("\n");

  if (0>status)
    {
      printf("sqlo_open2 failed at line %d: %s\n%s\n", __LINE__, 
             sqlo_geterror(dbh),
             select_stmt);
      return 0;
    }

  if (! (ocol_names = sqlo_ocol_names(sth, &n_ocols)))
    {
      printf("sqlo_ocol_names failed: %s\n", sqlo_geterror(sth));
      return 0;
    }


  if (! (ocol_name_lens = sqlo_ocol_name_lens(sth, NULL)))
    {
      printf("sqlo_ocol_name_lens failed: %s\n", sqlo_geterror(sth));
      return 0;
    }

  /* Print the select list data types */
  if ( 0 > (n_ocols = sqlo_ncols(sth, 0))) 
    {
      printf("sqlo_ncols failed: %s\n", sqlo_geterror(dbh));
      return 0;
    }

  for ( i = 1; i <= n_ocols; ++i) 
    {
      int dtype;
      if (0 > (dtype = sqlo_get_ocol_dtype(sth, i)))
        {
          printf("sqlo_get_ocol_dtype failed: %s\n", sqlo_geterror(dbh));
          return 0;
        }
      printf("do_select: Column: %d DataType: %d\n", i, dtype);
    }


  
  nrows = 0;
  while (0 == (status = sqlo_fetch(sth, 1)) || status == SQLO_STILL_EXECUTING)
    {
      if (status == SQLO_STILL_EXECUTING) {
        SQLO_USLEEP;
        continue;
      }

      v = sqlo_values(sth, NULL, 1);
      vl = sqlo_value_lens(sth, NULL);

      if ( 0 == nrows % 24 ) 
        {
          for (i = 0; i < n_ocols; ++i) 
            {
              printf("%-*s ", ocol_name_lens[i] > (int)vl[i] ?
                     ocol_name_lens[i] : (int)vl[i]
                     , ocol_names[i]);
            }
          printf("\n");
        }

      for (i = 0; i < n_ocols; ++i)
        {
          printf("%*s ", ocol_name_lens[i] > (int) vl[i] ?
                 ocol_name_lens[i] : (int) vl[i]
                 , v[i]);
        }
      printf("\n");
      /*      printf("\n%4s%6s%19s%21s%11s\n", v[0], v[1], v[2], v[3], v[4]);*/
      ++nrows;
    }

  if (status < 0)
    {
      printf("sqlo_fetch failed: %s\n", sqlo_geterror(dbh));
    }

#ifdef CLOSE_CURSOR
  if (0 > sqlo_close(sth))
    {
      printf("sqlo_close failed: %s\n", sqlo_geterror(dbh));
      return 0;
    }
#endif
  return 1;

}



/*-------------------------------------------------------------------------
 * Query the test table with sqlo_query_results
 *-----------------------------------------------------------------------*/
int test_sqlo_query_result( int dbh, int thread_id )
{
  int sth = SQLO_STH_INIT;
  int argc;
  const char *argv[1];
  char **v;
  unsigned short *vlens;
  int ncols;
  char **colnames;
  unsigned int *clens;

  int nrows;
  char sthread_id[16];

  int i;
  int status;

  char * select_stmt =
    "SELECT THREAD_ID THID, NKEY AS NUM_KEY, CKEY AS CHAR_KEY, TO_CHAR(NVAL,'9999999999.999') AS NVAL, CVAL, TO_CHAR(DVAL,'DD-MON-YYYY') AS DVAL, FVAL  FROM T_SQLORA_TEST WHERE THREAD_ID = :1";

  printf("test_sqlo_query_result: start\n");

  sprintf(sthread_id,"%d", thread_id);
  argc = 0;
  argv[argc++] = sthread_id;

  /* Select all and display */
  while (SQLO_STILL_EXECUTING == (status = sqlo_open2(&sth, dbh, select_stmt, argc, argv)))
    {
      printf(".");
      SQLO_USLEEP;
    }

  printf("\n");

  nrows = 0;
  while (0 == (status = sqlo_query_result(sth, &ncols, &v, &vlens, &colnames, &clens)) || 
	 status == SQLO_STILL_EXECUTING)
    {
      if (status == SQLO_STILL_EXECUTING) {
        SQLO_USLEEP;
        continue;
      }

      if ( 0 == nrows % 24 ) 
        {
          for (i = 0; i < ncols; ++i) 
	    {
              printf("%-*s ", greatest(clens[i], vlens[i]), colnames[i]  );
            }
          printf("\n");
        }

      for (i = 0; i < ncols; ++i)
        {
	  printf("%-*s ", greatest(clens[i], vlens[i]), v[i]  );
        }

      printf("\n");
      /*      printf("\n%4s%6s%19s%21s%11s\n", v[0], v[1], v[2], v[3], v[4]);*/
      ++nrows;
    }

  if (status < 0)
    {
      printf("sqlo_query_results failed: %s\n", sqlo_geterror(dbh));
    }

#ifdef CLOSE_CURSOR
  if (0 > sqlo_close(sth))
    {
      printf("sqlo_close failed: %s\n", sqlo_geterror(dbh));
      return 0;
    }
#endif
  printf("test_sqlo_query_result: finished\n");
  return 1;

}



/*-------------------------------------------------------------------------
 * Select with prepare/execute/fetch.
 *-----------------------------------------------------------------------*/
int test_select2( int dbh, int thread_id )
{
  int sth = SQLO_STH_INIT;

  /* Define a structure to hold one record */
  typedef struct _array_of_struct_t {
    int thread_id;
    int nkey;
    char ckey[6];
    double nval;
    char cval[21];
    char dval[11];
    char fval[128];
    unsigned short ckeyl;
    unsigned short cvall;
    unsigned short dvall;
    unsigned short fvall;
    unsigned short fvalrc;
  } aos_t;

  aos_t data[MAX_ARRAYSIZE];    /* A array of this structures */

  int status;
  int i;

  int rows_fetched = 0;
  int rows_fetched_total = 0;
  int rows_to_fetch;
  int done_fetching = 0;
  int ncols;
  int dtype;

  char * select_stmt =
    "SELECT THREAD_ID THID, NKEY, CKEY, NVAL, CVAL, TO_CHAR(DVAL,'DD-MM-YYYY') AS DVAL, FVAL FROM T_SQLORA_TEST WHERE THREAD_ID = :1";
  
  printf("Test select via classic methods\n");


  /* Insert test data */
  if (!test_array_insert(dbh, thread_id, 0))
    return 0;

  /* Select all and display */
  if (0>(sth = sqlo_prepare(dbh, select_stmt)))
    {
      printf("sqlo_prepare failed: %s\n", sqlo_geterror(dbh));
      return 0;
    }

  sqlo_set_prefetch_rows(sth, 2 * MAX_ARRAYSIZE);

  /* Bind input */
  if (SQLO_SUCCESS != 
      (sqlo_bind_by_name(sth, ":1", SQLOT_INT, &thread_id, sizeof(int), NULL, 0)))
    {
      printf("sqlo_bind_by_name failed: %s\n", sqlo_geterror(dbh));
      return 0;
    }

  /* Print the select list data types */
  if ( 0 > (ncols = sqlo_ncols(sth, 0))) 
    {
      printf("sqlo_ncols failed: %s\n", sqlo_geterror(dbh));
      return 0;
    }

  for ( i = 1; i <= ncols; ++i) 
    {
      if (0 > (dtype = sqlo_get_ocol_dtype(sth, i)))
        {
          printf("sqlo_get_ocol_dtype failed: %s\n", sqlo_geterror(dbh));
          return 0;
        }
      printf("Column: %d DataType: %d\n", i, dtype);
    }

  /* Define Output */
  
  if (SQLO_SUCCESS !=
      (sqlo_define_by_pos2(sth, 1, SQLOT_INT, &data[0].thread_id, sizeof(int),0, 0,
                           0, sizeof(aos_t)) ||
      sqlo_define_by_pos2(sth, 2, SQLOT_INT, &data[0].nkey, sizeof(int),0, 0, 
                          0, sizeof(aos_t)) ||
       sqlo_define_by_pos2(sth, 3, SQLOT_STR, data[0].ckey, sizeof(data[0].ckey), 0, 
                           &data[0].ckeyl, 0, sizeof(aos_t)) ||
       sqlo_define_by_pos2(sth, 4, SQLOT_FLT, &data[0].nval, sizeof(double),0, 0, 
                           0, sizeof(aos_t)) ||
       sqlo_define_by_pos2(sth, 5, SQLOT_STR, data[0].cval, sizeof(data[0].cval), 0, 
                           0, &data[0].cvall, sizeof(aos_t)) ||
       sqlo_define_by_pos2(sth, 6, SQLOT_STR, data[0].dval, sizeof(data[0].dval), 0, 
                           0, &data[0].dvall, sizeof(aos_t)) ||
       sqlo_define_by_pos2(sth, 7, SQLOT_STR, data[0].fval, sizeof(data[0].fval), 0, 
                           &data[0].fvall, &data[0].fvalrc, sizeof(aos_t))))
    {
      printf("sqlo_define_by_pos2 failed: %s\n", sqlo_geterror(dbh));
      return 0;
    }

  rows_to_fetch = 3;
  rows_fetched = rows_to_fetch;

  while ( SQLO_STILL_EXECUTING == (status = sqlo_execute(sth, rows_to_fetch)))
    {
      /* do something. */
      printf(".");
      SQLO_USLEEP;

    }
          
  printf("\n");

  if (status < 0)
    {
      printf("sqlo_execute failed (status=%d): %s\n", status, sqlo_geterror(dbh) );
      return(0);
    }
  else if (status == SQLO_NO_DATA)
    {
      /* arrays were filled fully. Get rowcount */
      rows_fetched = sqlo_prows(sth);
      done_fetching = 1;
      printf("Execute fetched all %d rows\n", rows_fetched);

      printf("Fetched all in one go\n");

      for (i = 0; i < rows_fetched; ++i)
        {
          printf("%4d %3d %5s %19f %20s %10s\n", 
                 data[i].thread_id, data[i].nkey, data[i].ckey, data[i].nval, 
                 data[i].cval, data[i].dval);
        }
    }
  
  for (i = 0; i < rows_fetched; ++i)
    {
      if (!i)
        printf("Execute fetched %d rows\n", rows_fetched);

          printf("%4d %3d %5s %19f %20s %10s\n", 
                 data[i].thread_id, data[i].nkey, data[i].ckey, data[i].nval, data[i].cval, 
                 data[i].dval);
    }

  rows_fetched_total += rows_fetched;
  rows_to_fetch = 4;

  while(!done_fetching)
    {
      rows_fetched = rows_to_fetch;
      while (SQLO_STILL_EXECUTING == (status = sqlo_fetch(sth, rows_to_fetch)))
        {
          printf(".");
	  SQLO_USLEEP;
        }

      printf("\n");

      if (status < 0)
        {
          printf("sqlo_fetch failed: %s\n", sqlo_geterror(dbh));
          return 0;
        }

      if (status ==  SQLO_NO_DATA)
        {
          rows_fetched = sqlo_prows(sth);
          
          /* The last call returns the total number of fetched rows
           * the difference to the previous total fechted rows is
           * the number of rows fetched in this last call to sqlo_execute
           */
          rows_fetched = rows_fetched - rows_fetched_total;
          done_fetching = 1;
          printf("sqlo_fetch fetched last %d rows\n", rows_fetched);
        }
      else if (status == SQLO_SUCCESS)
        {
          printf("sqlo_fetch fetched %d rows\n", rows_fetched);

        }
      else
        {
          printf("sqlo_fetch failed: %s\n", sqlo_geterror(dbh));
          return 0;
        }

      for (i = 0; i < rows_fetched; ++i)
        {
          printf("%4d %3d %5s %19f %20s %10s\n", 
                 data[i].thread_id, data[i].nkey, data[i].ckey, data[i].nval, data[i].cval, 
                 data[i].dval);
        }
      rows_fetched_total += rows_fetched;
    }
  printf("Fetched %d rows in total (should be %d)\n", rows_fetched_total, MAX_ARRAYSIZE+1);
#ifdef CLOSE_CURSOR
  if (0 > sqlo_close(sth))
    {
      printf("sqlo_close failed: %s\n", sqlo_geterror(dbh));
      return 0;
    }
#endif
  return 1;

}

/*-------------------------------------------------------------------------
 * test_reopen
 *-----------------------------------------------------------------------*/
int test_reopen( int dbh, int thread_id )
{
  int sth = SQLO_STH_INIT;
  const char **v;
  int argc;
  char sthread_id[16];
  const char *argv[1];
  int status;

  char * select_stmt =
    "SELECT NKEY, CKEY, NVAL, CVAL, TO_CHAR(DVAL,'DD-MM-YYYY') AS DVAL,FVAL FROM T_SQLORA_TEST WHERE THREAD_ID = :1";

  printf("Test reopen\n");

  sprintf(sthread_id, "%d", thread_id);
  argc = 0;
  argv[argc++] = sthread_id;

  /* Select all and display */
  while (SQLO_STILL_EXECUTING == 
      (status = sqlo_open2(&sth, dbh, select_stmt, argc, argv)))
    {
      printf(".");
      SQLO_USLEEP;
    }
  printf("\n");

  if (0>status)
    {
      printf("sqlo_open2 failed: %s\n", sqlo_geterror(dbh));
      return 0;
    }

  while (SQLO_STILL_EXECUTING  == (status = sqlo_fetch(sth, 1)))
    {
      printf(".");
      SQLO_USLEEP;
    }

  printf("\n");
  if (SQLO_SUCCESS == status)
    {
      do 
        {
          v = sqlo_values(sth, NULL, 1);
          printf("%s|%6s%19s%21s%11s%s\n", v[0], v[1], v[2], v[3], v[4], v[5]);
          
          while (SQLO_STILL_EXECUTING  == (status = sqlo_fetch(sth, 1)))
            {
              printf(".");
              SQLO_USLEEP;
            }

          printf("\n");
        } while (status == SQLO_SUCCESS);
    }

  argv[0] = sthread_id;

  while (SQLO_STILL_EXECUTING == (status = sqlo_reopen(sth, argc, argv)))
    {
      printf(".");
      SQLO_USLEEP;
    }
  printf("\n");

  if (SQLO_SUCCESS != status)
    {
      printf("sqlo_reopen failed: %s\n", sqlo_geterror(dbh));
      return 0;
    }

  printf("Fetch again\n");

  while (0 == (status = sqlo_fetch(sth,1)) || status == SQLO_STILL_EXECUTING)
    {
      if (status == SQLO_STILL_EXECUTING) {
        SQLO_USLEEP;
        continue;
      }
      v = sqlo_values(sth, NULL, 0);
      printf("%s|%6s%19s%21s%11s\n", v[0], v[1], v[2], v[3], v[4]);
    }

#ifdef CLOSE_CURSOR
  if (0 > sqlo_close(sth))
    {
      printf("sqlo_close failed: %s\n", sqlo_geterror(dbh));
      return 0;
    }
#endif
  printf("Test reopen ok\n");
  return 1;

}



/*-------------------------------------------------------------------------
 * Force a success with info return value and handle it.
 *-----------------------------------------------------------------------*/
int test_success_with_info( int dbh, int thread_id )
{
  int sth = SQLO_STH_INIT;
  const char **v;
  int nrows;
  int i;
  int status;

  char * select_stmt =
    "SELECT COUNT(MGR) FROM EMP";

  printf("test_success_with_info\n");

  /* Select all and display */
  while (SQLO_STILL_EXECUTING == (status = sqlo_open2(&sth, dbh, select_stmt, 0, NULL)))
    {
      printf(".");
      SQLO_USLEEP;
    }

  if (0>status)
    {
      printf("test_success_with_info: sqlo_open2 failed at line %d: %s\n%s\n", __LINE__, 
             sqlo_geterror(dbh),
             select_stmt);
      return 0;
    }

  nrows = 0;
  while (0 == (status = sqlo_fetch(sth, 1)) 
	 || status == SQLO_STILL_EXECUTING
	 || status == SQLO_SUCCESS_WITH_INFO
	 )
    {
      if (status == SQLO_STILL_EXECUTING) {
        SQLO_USLEEP;
        continue;
      }

      v = sqlo_values(sth, NULL, 1);
      printf("%s\n", v[0]);
      ++nrows;
    }

  printf("test_success_with_info: status=%d\n", status);
  if (status < 0)
    {
      printf("test_success_with_info: sqlo_fetch failed: %s\n", sqlo_geterror(dbh));
    }

#ifdef CLOSE_CURSOR
  if (0 > sqlo_close(sth))
    {
      printf("test_success_with_info: sqlo_close failed: %s\n", sqlo_geterror(dbh));
      return 0;
    }
#endif
  printf("test_success_with_info finished successfully\n");
  return 1;

}


/*-------------------------------------------------------------------------
 * create_packages
 *-----------------------------------------------------------------------*/
int create_packages( int dbh )
{
  int status;

  char * create_pack = 
    "CREATE OR REPLACE PACKAGE SQLORA_TEST IS\n"
    "  PROCEDURE P1(ip1 IN NUMBER, ip2 IN NUMBER, op1 OUT NUMBER, op2 OUT VARCHAR);\n"
    "END;\n";

  char * create_pack_body = 
    "CREATE OR REPLACE PACKAGE BODY SQLORA_TEST IS\n"
    "  PROCEDURE P1(ip1 IN NUMBER, ip2 IN NUMBER, op1 OUT NUMBER, op2 OUT VARCHAR)\n"
    "  IS \n"
    "  BEGIN\n"
    "     op1 := TO_NUMBER(ip1) + ip2;\n"
    "     op2 := TO_CHAR(op1);\n"
    "  END;\n"
    "END;\n";

  printf("Creating packages, dbh=%d\n", dbh);

  /* Check if the package already exists */
  if (SQLO_NO_DATA == 
      sqlo_exists(dbh, "USER_OBJECTS", "OBJECT_NAME", "SQLORA_TEST", "OBJECT_TYPE = 'PACKAGE'"))
    {
      printf("Create package SQLORA_TEST\n");
      while (SQLO_STILL_EXECUTING == (status = sqlo_exec(dbh, create_pack)))
        {
          printf(".");
          SQLO_USLEEP;
        }

      printf("\n");

      if ( 0 > status )
        {
          printf("sqlo_exec failed: %s\n%s\n",sqlo_geterror(dbh), create_pack );
          return 0;
        }
      printf("Package SQLORA_TEST created\n");
    }
  
  if (SQLO_NO_DATA == 
      sqlo_exists(dbh, "USER_OBJECTS", "OBJECT_NAME", "SQLORA_TEST", "OBJECT_TYPE = 'PACKAGE BODY'"))
    {
      printf("Create package body SQLORA_TEST\n");
      while (SQLO_STILL_EXECUTING == (status = sqlo_exec(dbh, create_pack_body)))
        {
          printf(".");
          SQLO_USLEEP;
        }
      printf("\n");

      if (0 > status)
        {
          printf("sqlo_exec failed: (status=%d) %s\n%s\n", status, sqlo_geterror(dbh), create_pack_body );
          return 0;
        }
      printf("Package body SQLORA_TEST created\n");
    }
  fflush(stdout);
  fflush(stderr);

  return 1;
}

/*-------------------------------------------------------------------------
 * test_plsql
 *-----------------------------------------------------------------------*/
int test_plsql( int dbh, int thread_id )
{
  int  ip2, op1;
  double ip1;
  char op2[40];
  int sth = SQLO_STH_INIT;
  int status;

  char * stmt = 
    "BEGIN\n"
    "    SQLORA_TEST.P1(:ip1, :ip2, :op1, :op2);\n"
    "END;\n";

  printf("test_plsql starts\n");

  if (!create_packages(dbh))
    return 0;

  ip1 = 1.123456789012345;
  ip2 = 20;
  op1 = 0;
  *op2 = 0;

  if (0 <= (sth = sqlo_prepare(dbh, stmt)))
    {
      if (SQLO_SUCCESS != 
          (sqlo_bind_by_name(sth, ":ip1", SQLOT_FLT, &ip1, sizeof(ip1),0,0) ||
           sqlo_bind_by_name(sth, ":ip2", SQLOT_INT, &ip2, sizeof(ip2),0,0) ||
           sqlo_bind_by_name(sth, ":op1", SQLOT_INT, &op1, sizeof(op1),0,0) ||
           sqlo_bind_by_name(sth, ":op2", SQLOT_STR, op2, sizeof(op2),0,0)
           ))
        {
          printf("sqlo_bind_by_name failed failed: %s\n", sqlo_geterror(dbh) );
          return 0;
        }
      else
        {
          while (SQLO_STILL_EXECUTING == (status = sqlo_execute(sth, 1)))
            {
              /* do something */
              printf(".");
              SQLO_USLEEP;
            }
          printf("\n");

          if (SQLO_SUCCESS != status) 
            {
              printf("sqlo_execute failed: %s\n", sqlo_geterror(dbh) );
              return 0;
            }
        }
#ifdef CLOSE_CURSOR
      if (SQLO_SUCCESS != sqlo_close(sth))
        {
          printf("sqlo_close failed: %s\n", sqlo_geterror(dbh) );
          return 0;
        }
#endif
      printf ("ip1: %.16f, ip2: %d, op1: %d, op2: %s\n", ip1, ip2, op1, op2);
    }
  else
    {
      printf("sqlo_prepare failed: Status: %d, %s\n", sth, sqlo_geterror(dbh) );
      return 0;
    }
  printf("test_plsql finished\n");
  return 1;
}

/*-------------------------------------------------------------------------
 * test_insert with bind by pos
 *-----------------------------------------------------------------------*/
int test_insert( int dbh, int thread_id, int delete_flag )
{
  int nkey;
  char ckey[6];
  double nval;
  char cval[21];
  char dval[12];
  int status;
  int sth = SQLO_STH_INIT;
  char fval[255];

  char * insert_stmt = 
    "INSERT INTO T_SQLORA_TEST (THREAD_ID, NKEY, CKEY, NVAL, CVAL, DVAL, FVAL) VALUES (:THR_ID, :NKEY, :CKEY, :NVAL, :CVAL, TO_DATE(:DVAL,'DD-MM-YYYY'), :FVAL)";



  printf("Testing Insert (bind by pos)\n");

  if (!create_table(dbh))
    return 0;


  nkey = 100;
  strcpy(ckey, "ckey");
  nval = 1234567890.001;
  strcpy(fval," 98765432109876543210987654321098765432109876543210.01");
  strcpy(cval,"aaaaaaaaaaaaaaaaaaaa");
  strcpy(dval,"01-JUL-2000");

  if (0 <= (sth = sqlo_prepare(dbh, insert_stmt)))
    {
      if (SQLO_SUCCESS != 
          (sqlo_bind_by_pos(sth, 1, SQLOT_INT, &thread_id, sizeof(int),0,0) ||
           sqlo_bind_by_pos(sth, 2, SQLOT_INT, &nkey, sizeof(int),0,0) ||
           sqlo_bind_by_pos(sth, 3, SQLOT_STR, ckey, sizeof(ckey),0,0) ||
           sqlo_bind_by_pos(sth, 4, SQLOT_FLT, &nval, sizeof(double),0,0) ||
           sqlo_bind_by_pos(sth, 5, SQLOT_STR, cval, strlen(cval)+1,0,0) ||
           sqlo_bind_by_pos(sth, 6, SQLOT_STR, dval, strlen(dval)+1,0,0) ||
           sqlo_bind_by_pos(sth, 7, SQLOT_STR, fval, strlen(fval)+1,0,0)
           ))
        {
          printf("sqlo_bind_by_pos failed at line %d: %s\n", __LINE__, sqlo_geterror(dbh) );
          return 0;
        }
      else
        {
          printf("Inserting the values\n");
          while (SQLO_STILL_EXECUTING == (status = sqlo_execute(sth, 1)))
            {
              /* do something */
              printf(".");
              SQLO_USLEEP;
            }
          printf("\n");
          if (SQLO_SUCCESS != status)
            {
              printf("sqlo_execute failed at line %d: %s\n", 
                     __LINE__, sqlo_geterror(dbh) );
              return 0;
            }
        }
#ifdef CLOSE_CURSOR
      if (SQLO_SUCCESS != sqlo_close(sth))
        {
          printf("sqlo_close failed: %s\n", sqlo_geterror(dbh) );
          return 0;
        }
#endif

      if (delete_flag)
        if (!do_select(dbh, thread_id))
          return 0;
    }
  else
    {
      printf("sqlo_prepare failed: Status: %d, %s\n", sth, sqlo_geterror(dbh) );
      return 0;
    }

  if (delete_flag)
    if (!delete_table(dbh, "T_SQLORA_TEST", thread_id))
      return(0);

  printf("finished test_insert\n");
  return 1;
}

/*-------------------------------------------------------------------------
 * test_array_insert
 *-----------------------------------------------------------------------*/
int test_array_insert( int dbh, int thread_id, int delete_flag )
{
  int nkey[MAX_ITERS];
  char ckey[MAX_ITERS][6];
  double nval[MAX_ITERS];
  char cval[MAX_ITERS][21];
  char dval[MAX_ITERS][16];
  char fval[MAX_ITERS][255];

  short nind[MAX_ITERS];
  short cind[MAX_ITERS];
  short dind[MAX_ITERS];
  short find[MAX_ITERS];
  int thread_ids[MAX_ITERS];

  int sth, i, j;
  int status;

  char * insert_stmt = 
    "INSERT INTO T_SQLORA_TEST (THREAD_ID, NKEY, CKEY, NVAL, CVAL, DVAL, FVAL) VALUES (:THR_ID, :NKEY, :CKEY, :NVAL, :CVAL, TO_DATE(:DVAL,'DD-MM-YYYY'), :FVAL)";

  printf("Testing Array Insert (bind by name)\n");


  if (!create_table(dbh))
    return 0;

  /* setup bind arrays */
  for ( i = 0 ; i < MAX_ITERS; i++)
    {
      nkey[i] = i+1;
      sprintf(ckey[i], "%c", 'A' + i % 26 );
      nval[i] = 1234567890.0 + i / 1000.0;
      strcpy(fval[i],"123456789012345678901234567890123456789012345678901234567890");
      for (j = 0; j < 20; j++)
        cval[i][j] = 'a' + i % 26;
      cval[i][20] = '\0';
      sprintf(dval[i], "%02d-JUL-2000", (i % 30 ) + 1);
      
      nind[i] = 0;
      cind[i] = 0;
      dind[i] = 0;
      find[i] = 0;
      thread_ids[i] = thread_id;
    }

  if (0 <= (sth = sqlo_prepare(dbh, insert_stmt)))
    {
      if (SQLO_SUCCESS != 
          (sqlo_bind_by_name(sth, ":NKEY", SQLOT_INT, &nkey[0], sizeof(int), NULL,1) ||
           sqlo_bind_by_name(sth, ":CKEY", SQLOT_STR, &ckey[0], 6, NULL,1) ||
           sqlo_bind_by_name(sth, ":NVAL", SQLOT_FLT, &nval[0], sizeof(double), nind,1) ||
           sqlo_bind_by_name(sth, ":CVAL", SQLOT_STR, &cval[0], 21, cind,1) ||
           sqlo_bind_by_name(sth, ":DVAL", SQLOT_STR, &dval[0], 16, dind,1) ||
           sqlo_bind_by_name(sth, ":THR_ID", SQLOT_INT, &thread_ids[0], sizeof(int), NULL,1) ||
           sqlo_bind_by_name(sth, ":FVAL", SQLOT_STR, &fval[0], 255, find,1)
           ))
        {
          printf("sqlo_bind_by_name failed failed: %s\n", sqlo_geterror(dbh) );
          return 0;
        }
      else
        {
          while (SQLO_STILL_EXECUTING == (status = sqlo_execute(sth, MAX_ITERS)))
            {
              /* do something */
              printf(".");
              SQLO_USLEEP;
            }

          printf("\n");

          if (SQLO_SUCCESS != status)
            {
              printf("sqlo_execute failed: %s\n", sqlo_geterror(dbh) );
              return 0;
            }
        }
#ifdef CLOSE_CURSOR
      if (SQLO_SUCCESS != sqlo_close(sth))
        {
          printf("sqlo_close failed: %s\n", sqlo_geterror(dbh) );
          return 0;
        }
#endif
      if (delete_flag)
        if (!do_select(dbh, thread_id))
          return 0;
    }
  else
    {
      printf("sqlo_prepare failed: Status: %d, %s\n", sth, sqlo_geterror(dbh) );
      return 0;
    }

  if (delete_flag)
    if (!delete_table(dbh, "T_SQLORA_TEST", thread_id))
      return(0);

  return 1;
}


/*-------------------------------------------------------------------------
 * test_array_insert2 (by pos)
 *-----------------------------------------------------------------------*/
int test_array_insert2( int dbh, int thread_id, int delete_flag )
{
  int nkey[MAX_ITERS];
  char ckey[MAX_ITERS][6];
  double nval[MAX_ITERS];
  char cval[MAX_ITERS][21];
  char dval[MAX_ITERS][16];
  double fval[MAX_ITERS];

  short nind[MAX_ITERS];
  short cind[MAX_ITERS];
  short dind[MAX_ITERS];
  short find[MAX_ITERS];

  int thread_ids[MAX_ITERS];

  int i, j;
  int status;
  int sth;

  char * insert_stmt = 
    "INSERT INTO T_SQLORA_TEST (THREAD_ID, NKEY, CKEY, NVAL, CVAL, DVAL, FVAL) VALUES (:THR_ID, :NKEY, :CKEY, :NVAL, :CVAL, TO_DATE(:DVAL,'DD-MM-YYYY'), :FVAL)";

  printf("Testing Array Insert ( bind by pos)\n");


  if (!create_table(dbh))
    return (0);

  /* setup bind arrays */
  for ( i = 0 ; i < MAX_ITERS; i++)
    {
      nkey[i] = i+1;
      sprintf(ckey[i], "%c", 'A' + i % 26 );
      nval[i] = 1234567890.0 + i / 1000.0;
      fval[i] = nval[i];
      for (j = 0; j < 20; j++)
        cval[i][j] = 'a' + i % 26;
      cval[i][20] = '\0';
      sprintf(dval[i], "%02d-JUL-2000", (i % 30) + 1);
      
      nind[i] = 0;
      cind[i] = 0;
      dind[i] = 0;
      find[i] = 0;
      thread_ids[i] = thread_id;
    }

  if (0 <= (sth = sqlo_prepare(dbh, insert_stmt)))
    {
      if (SQLO_SUCCESS != 
          (sqlo_bind_by_pos(sth, 1, SQLOT_INT, &thread_ids[0], sizeof(int), NULL,1) ||
           sqlo_bind_by_pos(sth, 2, SQLOT_INT, &nkey[0], sizeof(int), NULL,1) ||
           sqlo_bind_by_pos(sth, 3, SQLOT_STR, &ckey[0], 6, NULL,1) ||
           sqlo_bind_by_pos(sth, 4, SQLOT_FLT, &nval[0], sizeof(double), nind,1) ||
           sqlo_bind_by_pos(sth, 5, SQLOT_STR, &cval[0], 21, cind,1) ||
           sqlo_bind_by_pos(sth, 6, SQLOT_STR, &dval[0], 16, dind,1) ||
           sqlo_bind_by_pos(sth, 7, SQLOT_FLT, &fval[0], sizeof(double), find,1) 
           ))
        {
          printf("sqlo_bind_by_pos failed failed: %s\n", sqlo_geterror(dbh) );
          return 0;
        }
      else
        {
          while (SQLO_STILL_EXECUTING == (status = sqlo_execute(sth, MAX_ITERS)))
            {
              /* do something */
              printf(".");
              SQLO_USLEEP;
            }

          printf("\n");
          if (SQLO_SUCCESS != status)
            {
              printf("sqlo_execute failed: %s\n", sqlo_geterror(dbh) );
              return 0;
            }
        }
#ifdef CLOSE_CURSOR
      if (SQLO_SUCCESS != sqlo_close(sth))
        {
          printf("sqlo_close failed: %s\n", sqlo_geterror(dbh) );
          return 0;
        }
#endif
      if (delete_flag)
        if (!do_select(dbh, thread_id))
          return 0;
    }
  else
    {
      printf("sqlo_prepare failed: Status: %d, %s\n", sth, sqlo_geterror(dbh) );
      return 0;
    }

  if (delete_flag)
    if (!delete_table(dbh, "T_SQLORA_TEST", thread_id))
      return(0);

  return 1;
}


/*-------------------------------------------------------------------------
 * test_exists
 *-----------------------------------------------------------------------*/
int test_exists(int dbh, int thread_id)
{
  int status;
  char sthread_id[16];
  sprintf(sthread_id,"%d", thread_id);


  /* Record does not exist */
  if (SQLO_SUCCESS == 
      (status = sqlo_exists(dbh, "T_SQLORA_TEST", "THREAD_ID", "-1", NULL)))
    printf("test_exists failed\n");
  else
    {
      if (status != SQLO_NO_DATA)
        {
          printf("test_exists failed: %s\n", sqlo_geterror(dbh));
          return 0;
        }
      else
        printf("test_exists  ok\n");
        
    }

  return 1;
}

/*-------------------------------------------------------------------------
 * test_count
 *-----------------------------------------------------------------------*/
int test_count(int dbh, int thread_id)
{
  int count;
  char sthread_id[16];
  sprintf(sthread_id,"%d", thread_id);

  if (!test_insert(dbh, thread_id, 0))
    return(0);

  if ( 0 <= (count = sqlo_count(dbh, "T_SQLORA_TEST", "THREAD_ID", sthread_id, NULL)))
    printf("test_count(1) ok\n");
  else
    {
      printf("test_count(1) failed: %s\n", sqlo_geterror(dbh));
      return 0;
    }

  return 1;
}


/*-------------------------------------------------------------------------
 * test_exec
 * Tests sqlo_exec with sqlo_break 
 *-----------------------------------------------------------------------*/
int test_exec(int dbh, int thread_id)
{
  int status;
  int retval = 1;
  int i;
  char * stmt = "ALTER SESSION SET SQL_TRACE FALSE";
  int bf = 0;
  int sth = SQLO_STH_INIT;
  char * stmt2 = "SELECT TABLE_NAME FROM ALL_TABLES WHERE ROWNUM <= 3";
  const char **v;

  printf("test_exec ");

  
  while (SQLO_STILL_EXECUTING == (status = sqlo_open2(&sth, dbh, stmt2, 0, NULL)))
    {
      printf(".");
      SQLO_USLEEP;
    }

  if (0 > status)
    {
      printf("sqlo_open2 returned Error %s\n%s\n", sqlo_geterror(dbh),
             stmt2);
      return (0);
    }

  for (i = 0; i < 1000; ++i)
    {
      while (SQLO_STILL_EXECUTING ==  (status = sqlo_exec(dbh, stmt)))
        {
          int status2;


          printf(".");

          if ( i == 998 && !bf )
            {
              printf("Testing break\n");
              status2 = sqlo_break(dbh);
              if (0 > status2)
                {
                  printf("sqlo_break failed (status=%d): %s\n", status2, sqlo_geterror(dbh));
                  retval = 0;
                }
              bf = 1;

              if (retval)
                printf("Testing break ok\n");
              else
                printf("Testing break failed\n");
            }
        }

      if ( 0 > status)
        {
          printf("sqlo_exec failed at %d (status=%d): %s\n", i, status, sqlo_geterror(dbh));
          return(0);
        }
    } /* end for */

  /* Lets see if we can continue fetch from the opened cursor after
   * we called sqlo_break */
  while (SQLO_STILL_EXECUTING == (status = sqlo_fetch(sth, 1)))
    {
      printf(".");
      SQLO_USLEEP;
    }

  if (0 > status)
    {
      printf("sqlo_fetch failed: %s\n", sqlo_geterror(dbh));
      retval = 0;
    }
  else
    {
      do {
        v = sqlo_values(sth, NULL, 1);
        printf("Table %s\n", v[0]);
        while (SQLO_STILL_EXECUTING == (status = sqlo_fetch(sth, 1)))
          {
            printf(".");
            SQLO_USLEEP;
          }
      } while (status == SQLO_SUCCESS);
      
      if (status != SQLO_NO_DATA)
        {
          printf("fetch vailed %s\n", sqlo_geterror(dbh));
          retval = 0;
        }
    }

  if (retval)
    printf("Test sqlo_exec ok\n");
  else
    printf("Test sqlo_exec failed\n");

  return retval;
}

/*-------------------------------------------------------------------------
 * create_tables(dbh)
 *-----------------------------------------------------------------------*/
int create_tables(int dbh)
{
  if (!create_table(dbh))
    return(0);

  if (!create_table_long(dbh))
    return(0);

  if (!create_packages(dbh))
    return (0);

  return(1);
}

/*-------------------------------------------------------------------------
 * int cleanup
 *-----------------------------------------------------------------------*/
int cleanup(int dbh)
{
  /* ignore all errors maybe they weren't created */
  while (SQLO_STILL_EXECUTING == sqlo_exec(dbh, "DROP Table T_SQLORA_TEST"))
    {
      printf(".");
      SQLO_USLEEP;
    }

  while (SQLO_STILL_EXECUTING == sqlo_exec(dbh, "DROP TABLE T_SQLORA_TEST_LONG"))
    {
      printf(".");
      SQLO_USLEEP;
    }


  while (SQLO_STILL_EXECUTING == sqlo_exec(dbh, "DROP TABLE T_SQLORA_TEST_CLOB"))
    {
      printf(".");
      SQLO_USLEEP;
    }

  while (SQLO_STILL_EXECUTING == sqlo_exec(dbh, "DROP PACKAGE BODY SQLORA_TEST"))
    {
      printf(".");
      SQLO_USLEEP;
    }


  while (SQLO_STILL_EXECUTING == sqlo_exec(dbh, "DROP PACKAGE SQLORA_TEST"))
    {
      printf(".");
      SQLO_USLEEP;
    }
  return 1;
}



/*-------------------------------------------------------------------------
 * test_autocommit
 *-----------------------------------------------------------------------*/
int test_autocommit(int dbh, int thread_id)
{
  char const * argv[1];
  char sthread_id[20];
  int cnt;

  printf("Test Autocommit\n");

  sprintf( sthread_id, "%d", thread_id);
  argv[0] = sthread_id;

  if (0> sqlo_run( dbh, "DELETE T_SQLORA_TEST WHERE THREAD_ID = :b1", 1, argv))
    {
      printf("sqlo_run (DELETE) failed: %s\n", sqlo_geterror(dbh));
      return (0);
    }

  if ( SQLO_SUCCESS != sqlo_set_autocommit( dbh, SQLO_ON ) )
    {
      printf("sqlo_set_autocommit(ON) failed: %s\n", sqlo_geterror(dbh));
      return (0);
    }

  if (SQLO_ON != sqlo_autocommit( dbh ) )
    {
      printf("sqlo_autocommit: Autocommit is off, but we expected it to be on\n");
      return (0);
    }

  /* Insert one record with autocommit */
  if (0> sqlo_run( dbh, "INSERT INTO T_SQLORA_TEST (THREAD_ID) VALUES (:b1) ", 1, argv))
    {
      printf("sqlo_run (INSERT) failed: %s\n", sqlo_geterror(dbh));
      return (0);
    }

  /* Do a rollback, but the data should be stored! */
  sqlo_rollback( dbh );
  
  cnt = sqlo_count( dbh, "T_SQLORA_TEST", "THREAD_ID", sthread_id, NULL);
  if ( cnt < 0 )
    {
      printf("sqlo_count failed: %s\n", sqlo_geterror(dbh));
      return (0);
    }
  else if ( cnt != 1 )
    {
      printf("test_autocommit failed!!!\n");
      return (0);
    }

  /* switch autocommit off */
  if ( SQLO_SUCCESS != sqlo_set_autocommit( dbh, SQLO_OFF ) )
    {
      printf("sqlo_set_autocommit(OFF) failed: %s\n", sqlo_geterror(dbh));
      return (0);
    }

  if (SQLO_OFF != sqlo_autocommit( dbh ) )
    {
      printf("sqlo_autocommit: Autocommit is on, but we expected it to be off\n");
      return (0);
    }

  return (1);
}


/*-------------------------------------------------------------------------
 * run_test(void * arg);
 *-----------------------------------------------------------------------*/
#if defined(ENABLE_PTHREADS)  && defined(HAVE_PTHREAD_H)
void * run_test(void * arg)
#else
int run_test(int arg)
#endif
{
  int status;
  int i = (int) arg;
  int thread_id;
  int session_created;
  int server_attached;
  int tries;
#if defined(ENABLE_PTHREADS)  && defined(HAVE_PTHREAD_H)
  thread_id = pthread_self();
#else
  thread_id = 1;
#endif

  tries = 0;
  server_attached = 0;
  do {
    tries++;
    if (SQLO_SUCCESS != (status = sqlo_server_attach(&g_dbh[i], g_cstr))) {
      printf("sqlo_server_attach: failed %s\n", sqlo_geterror(g_dbh[i]));
      sleep(5);
    } else {
      server_attached = 1;
    }
  } while (!server_attached && tries < 5);

  if (SQLO_SUCCESS == status)
    {
#if defined(ENABLE_PTHREADS)  && defined(HAVE_PTHREAD_H)
      printf("Thread %d attached to server. g_dbh[i]=%d\n", (int) pthread_self(), g_dbh[i]);
#else
      printf("Attached to server. g_dbh[i]=%d\n", g_dbh[i]);
#endif
    }
  else
    {
      printf("connect failed with status: %d\n%s", status, sqlo_geterror(g_dbh[i]));
#if defined(ENABLE_PTHREADS)  && defined(HAVE_PTHREAD_H)
      pthread_exit((void *) 1);
#else
      return 1;
#endif

    }
  
  session_created = 0;
  do {
    if ( SQLO_SUCCESS != (status = sqlo_session_begin(g_dbh[i], g_cstr, ""))) {
      printf("sqlo_session_begin: failed for dbh=%d %s\n", g_dbh[i],
             sqlo_geterror(g_dbh[i]));
      sleep(5);
    } else {
      session_created = 1;
    }

  } while (!session_created);

  /* make sure we use the right number format */
  if (SQLO_SUCCESS != sqlo_exec(g_dbh[i], "ALTER SESSION SET NLS_TERRITORY='America'"))
    {
      printf("SQLERROR: %s\n", sqlo_geterror(g_dbh[i]));
      return 0;
    }

  if (g_nonblocking_mode)
    {
      printf("Running in non-blocking mode\n");
      if (SQLO_SUCCESS != sqlo_set_blocking(g_dbh[i], 0))
        {
          printf("Could not switch to non-blocking mode %s\n",
                 sqlo_geterror(g_dbh[i]));
#if defined(ENABLE_PTHREADS)  && defined(HAVE_PTHREAD_H)
          pthread_exit((void *) 1);
#else
          return 1;
#endif
          
        }
    }
  else
    {
      printf("Running in blocking mode\n");
    }

  /* Set the date format */
  sqlo_exec(g_dbh[i], "ALTER SESSION SET NLS_DATE_FORMAT='DD.MM.YYYY';");

#if defined(ENABLE_PTHREADS)  && defined(HAVE_PTHREAD_H)
  pthread_mutex_lock(&g_create_tables_lock);
#endif

  if (!create_tables(g_dbh[i]))
    {
#if defined(ENABLE_PTHREADS)  && defined(HAVE_PTHREAD_H)
      pthread_mutex_unlock(&g_create_tables_lock);
      pthread_exit((void*) 1);
#else
      return 0;
#endif
    }

#if defined(ENABLE_PTHREADS)  && defined(HAVE_PTHREAD_H)
  pthread_mutex_unlock(&g_create_tables_lock);
#endif

  if (!test_plsql(g_dbh[i], thread_id))
#if defined(ENABLE_PTHREADS)  && defined(HAVE_PTHREAD_H)
    pthread_exit((void *) 1);
#else
  return 1;
#endif

  if (!test_insert(g_dbh[i], thread_id, 1))
#if defined(ENABLE_PTHREADS)  && defined(HAVE_PTHREAD_H)
    pthread_exit((void *) 1);
#else
  return 1;
#endif


  if (!test_array_insert(g_dbh[i], thread_id, 1)) /* bind by name */
#if defined(ENABLE_PTHREADS)  && defined(HAVE_PTHREAD_H)
    pthread_exit((void *) 1);
#else
  return 1;
#endif

  if (!test_array_insert2(g_dbh[i], thread_id, 1)) /* bind by pos */
#if defined(ENABLE_PTHREADS)  && defined(HAVE_PTHREAD_H)
    pthread_exit((void *) 1);
#else
  return 1;
#endif

  if (!test_exists(g_dbh[i], thread_id))
#if defined(ENABLE_PTHREADS)  && defined(HAVE_PTHREAD_H)
    pthread_exit((void *) 1);
#else
  return 1;
#endif

  if (!test_count(g_dbh[i], thread_id))
#if defined(ENABLE_PTHREADS)  && defined(HAVE_PTHREAD_H)
    pthread_exit((void *) 1);
#else
  return 1;
#endif

  if (!test_reopen(g_dbh[i], thread_id))
#if defined(ENABLE_PTHREADS)  && defined(HAVE_PTHREAD_H)
    pthread_exit((void *) 1);
#else
  return 1;
#endif

  if (!test_select2(g_dbh[i], thread_id))
#if defined(ENABLE_PTHREADS)  && defined(HAVE_PTHREAD_H)
    pthread_exit((void *) 1);
#else
  return 1;
#endif

  if (!test_sqlo_query_result(g_dbh[i], thread_id))
#if defined(ENABLE_PTHREADS)  && defined(HAVE_PTHREAD_H)
    pthread_exit((void *) 1);
#else
  return 1;
#endif

  if (!test_long(g_dbh[i], thread_id))
#if defined(ENABLE_PTHREADS)  && defined(HAVE_PTHREAD_H)
    pthread_exit((void *) 1);
#else
  return 1;
#endif

  if (!test_clob(g_dbh[i], thread_id))
#if defined(ENABLE_PTHREADS)  && defined(HAVE_PTHREAD_H)
    pthread_exit((void *) 1);
#else
  return 1;
#endif

  /* Test a query which returns success with info*/
  if (!test_success_with_info(g_dbh[i], thread_id))
#if defined(ENABLE_PTHREADS)  && defined(HAVE_PTHREAD_H)
    pthread_exit((void *) 1);
#else
  return 1;
#endif

  printf("Rollback ");
  if (SQLO_SUCCESS != (status = sqlo_rollback(g_dbh[i])))
    printf("rollback failed (%d): %s\n", status, sqlo_geterror(g_dbh[i]));
  printf("ok\n");
  
  /* Test a lot of sqlo_exec calls */
  if (!test_exec(g_dbh[i], thread_id))
#if defined(ENABLE_PTHREADS)  && defined(HAVE_PTHREAD_H)
    pthread_exit((void *) 1);
#else
  return 1;
#endif

  /* Test autocommit */
  if (!test_autocommit(g_dbh[i], thread_id))
#if defined(ENABLE_PTHREADS)  && defined(HAVE_PTHREAD_H)
    pthread_exit((void *) 1);
#else
  return 1;
#endif


  sqlo_finish(g_dbh[i]);

#if defined(ENABLE_PTHREADS)  && defined(HAVE_PTHREAD_H)
  pthread_exit((void *) 0);
#else
  return 0;
#endif

}


/*-------------------------------------------------------------------------
 * start_test
 *-----------------------------------------------------------------------*/
static int start_test(void)
{
  int i;

#if defined(ENABLE_PTHREADS)  && defined(HAVE_PTHREAD_H)
  pthread_mutex_init(&g_create_tables_lock, NULL);
#endif
  for (i = 0; i < MAX_LOOPS; i++)
    {
#if defined(ENABLE_PTHREADS)  && defined(HAVE_PTHREAD_H)
      pthread_create(&g_thr[i], NULL, run_test, (void *) i);
#else
      if (run_test( i ))
        return 1;
#endif
    }
  return(0);
}

/*-------------------------------------------------------------------------
 * join_threads
 *-----------------------------------------------------------------------*/
static int join_threads(void)
{
#if defined(ENABLE_PTHREADS)  && defined(HAVE_PTHREAD_H)
  int i;
  int thr_status;

  /* wait for the threads to complete */
  for (i = 0; i < MAX_LOOPS; i++)
    {
      pthread_join(g_thr[i], (void **) &thr_status);
      printf("Thread %ld finished with %d\n",
             g_thr[i], thr_status);
    }
#endif
  return (0);
}



/*=========================================================================
 *  main
 *=======================================================================*/
int main (int argc, char * argv[]) 
{
  int status;
  int dbh;
  char server_version[128];

  /*
  signal(SIGALRM, sig_handler);
  alarm(20);
  */
  printf("-------------------------------------------------------------\n\n");

  if (argc > 1)
    g_cstr = argv[1];
  else
    g_cstr = "scott/tiger";

  
#if defined(ENABLE_PTHREADS)  && defined(HAVE_PTHREAD_H)
  status = sqlo_init(1, MAX_LOOPS+1, 5);
#else
  status = sqlo_init(0, MAX_LOOPS+1, 5);
#endif
  
  if (SQLO_SUCCESS != status)
    {
      printf ("sqlo_init failed. Exiting\n");
      exit(1);
    }

  status = sqlo_connect(&dbh, g_cstr);

  if (SQLO_SUCCESS != status) 
    {
      printf ("sqlo_connect failed. Exiting\n");
      exit(1);
    }
    
  status = sqlo_server_version(dbh, server_version, sizeof(server_version));

  if (SQLO_SUCCESS == status)
    printf("Server version=%s", server_version);

  /* start test */
  if (start_test())
    {
      exit(1);
    }

  join_threads();

  cleanup(dbh);

#ifdef TEST_NONBLOCKING
  g_nonblocking_mode=1;
  printf("*** RUN ALL TESTS IN NON-BLOCKING MODE ***\n");

  /* start test */
  if (start_test())
    {
      exit (1);
    }

  join_threads();


  cleanup(dbh);
#endif

  sqlo_finish(dbh);
  printf("Finished tests.\nDid the output look like expected?\n");

  return (0);
}

