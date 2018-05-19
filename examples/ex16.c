/* $Id: ex16.c 286 2004-06-15 10:15:52Z kpoitschke $ */
#include <stdio.h>
#include <stdlib.h>
#include "examples.h"

int select_file_from_blob_table(sqlo_db_handle_t dbh, int key, const char *fname )
{
  char * stmt = 
    "SELECT KEY, BDATA FROM T_SQLORA_BLOB WHERE KEY = :1";
  int status;
  sqlo_stmt_handle_t sth = SQLO_STH_INIT;
  int k = key;
  FILE * fp;
  sqlo_lob_desc_t loblp;
  unsigned int loblen;
  short ind;

  if (!(fp = fopen(fname, "w"))) {
    printf("ERROR: Cannot open %s for write\n", fname);
    return 0;
  }

  /* parse */
  if (0 > (sth = sqlo_prepare(dbh, stmt)))
    error_exit(dbh, "sqlo_prepare");

  /* bind input */
  if (SQLO_SUCCESS != 
      (sqlo_bind_by_pos(sth, 1, SQLOT_INT, &k, sizeof(k), 0, 0)))
    error_exit(dbh, "sqlo_bind_by_pos");

  /* alloc lob desc */
  if (SQLO_SUCCESS !=  sqlo_alloc_lob_desc(dbh, &loblp))
    error_exit(dbh, "sqlo_alloc_lob_desc");

  /* define output */
  if (SQLO_SUCCESS != 
      (sqlo_define_by_pos(sth, 1, SQLOT_INT, &k, sizeof(k), 0, 0, 0)) ||
      (sqlo_define_by_pos(sth, 2, SQLOT_BLOB, &loblp, 0, &ind, 0, 0))) {
    sqlo_free_lob_desc(dbh, &loblp);
    error_exit(dbh, "sqlo_define_by_pos2");
  }      

  /* execute */
  status = sqlo_execute(sth, 1);

  if ( 0 > status) {
    sqlo_free_lob_desc(dbh, &loblp);
    error_exit(dbh, "sqlo_execute");
  }

  /* get the LOB length */
  status = sqlo_lob_get_length(dbh, loblp, &loblen);

  if ( 0 > status) {
    sqlo_free_lob_desc(dbh, &loblp);
    error_exit(dbh, "sqlo_log_get_length");
  }

  /* write it to the file */
  status = sqlo_lob_read_stream(dbh, loblp, loblen, fp);

  if ( 0 > status) {
    sqlo_free_lob_desc(dbh, &loblp);
    sqlo_close(sth);
    error_exit(dbh, "sqlo_lob_read_stream");
  }

  fclose(fp);

  sqlo_free_lob_desc(dbh, &loblp);
  sqlo_close(sth);

  sqlo_rollback(dbh);
  drop_blob_table(dbh);

  return (1);

}

/* $Id: ex16.c 286 2004-06-15 10:15:52Z kpoitschke $ */
