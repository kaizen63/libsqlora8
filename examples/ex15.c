/* $Id: ex15.c 286 2004-06-15 10:15:52Z kpoitschke $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "examples.h"

int select_from_blob_table(sqlo_db_handle_t dbh, int key )
{
  char * stmt = 
    "SELECT KEY, CDATA FROM T_SQLORA_BLOB WHERE KEY = :1";
  int status;
  sqlo_stmt_handle_t sth = SQLO_STH_INIT;
  int k = key;
  char * data;
  short ind;
  char cmp_data[MAX_BLOB_BUFFER_DATA];        /* our buffer to compare the result */
  sqlo_lob_desc_t loblp;
  unsigned int loblen;

  printf("Query CLOB\n");

  fillbuf2(cmp_data, MAX_BLOB_BUFFER_DATA); /* our reference data */

  /* parse */
  if (0 > (sth = sqlo_prepare(dbh, stmt)))
    error_exit(dbh, "sqlo_prepare");

  /* bind input */
  if (SQLO_SUCCESS != 
      (sqlo_bind_by_pos(sth, 1, SQLOT_INT, &k, sizeof(k), 0, 0))) {
      error_exit(dbh, "sqlo_bind_by_pos");
    }
      
  /* allocate a lob descriptor */
  if (0 > sqlo_alloc_lob_desc(dbh, &loblp))
    error_exit(dbh, "sqlo_alloc_lob_desc");

  /* define output */
  if (SQLO_SUCCESS != 
      (sqlo_define_by_pos(sth, 1, SQLOT_INT, &k, sizeof(k), 0, 0, 0)) ||
      (sqlo_define_by_pos(sth, 2, SQLOT_CLOB, &loblp, 0, &ind, 0, 0))) {

      sqlo_free_lob_desc(dbh, &loblp);
      error_exit(dbh, "sqlo_define_by_pos2");
    }      

  /* execute */
  status = sqlo_execute(sth, 1);

  if (SQLO_SUCCESS != status)  {
    sqlo_free_lob_desc(dbh, &loblp);
    error_exit(dbh, "sqlo_execute");
  }

  if (ind != SQLO_NULL_IND) 
    {
      status = sqlo_lob_get_length(dbh, loblp, &loblen);

      if ( 0 > status) {
        sqlo_free_lob_desc(dbh, &loblp);
        error_exit(dbh, "sqlo_free_lob_desc");
      }

      if (loblen != MAX_BLOB_BUFFER_DATA) {
        printf("Invalid LOB size. Expected %d, got %d\n", MAX_BLOB_BUFFER_DATA,
               loblen);
        sqlo_free_lob_desc(dbh, &loblp);
        sqlo_close(sth);
        return 0;
      }

      /* allocate the buffer for the data */
      data = malloc(loblen * sizeof(char));
      if (!data) {
          printf("FATAL: malloc error at %d\n", __LINE__);
          sqlo_free_lob_desc(dbh, &loblp);
          exit(EXIT_FAILURE);
      }

      /* read the data into the buffer */
      status = sqlo_lob_read_buffer(dbh, loblp, loblen, data, loblen);

      if ( 0 > status)  {
          printf("sqlo_lob_read_buffer failed: %s\n", sqlo_geterror(dbh) );
          sqlo_free_lob_desc(dbh, &loblp);
          error_exit(dbh, "sqlo_lob_read_buffer");
        }

      printf("Compare CLOB\n");
      /* compare with our reference data */
      if (memcmp(data, &cmp_data, MAX_BLOB_BUFFER_DATA)) {
        int i;
        printf("LOB read is different from LOB written\n");
        for (i = 0; i <= MAX_BLOB_BUFFER_DATA; ++i) {
          if (data[i] != cmp_data[i])
            printf("diff at pos %d\n", i);
        }
      }

      if (data)
	free(data);
      data = NULL;
    } else {
      printf("LOB is NULL\n");
      return 0;
    }

  sqlo_free_lob_desc(dbh, &loblp);
  sqlo_close(sth);

  return (1);

}

/* $Id: ex15.c 286 2004-06-15 10:15:52Z kpoitschke $ */
