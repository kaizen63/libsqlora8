/* $Id: ex14.c 286 2004-06-15 10:15:52Z kpoitschke $ */
#include <stdio.h>
#include <stdlib.h>
#include "examples.h"

int insert_file_into_blob_table(sqlo_db_handle_t dbh, int key, const char * fname )
{

  char * stmt = 
    "INSERT INTO T_SQLORA_BLOB (KEY, BDATA) "
    "VALUES (:b1, EMPTY_BLOB()) RETURNING BDATA INTO :b2";
  sqlo_lob_desc_t loblp;                  /* the lob locator */
  sqlo_stmt_handle_t sth;
  int status;
  FILE * fp;
  unsigned filelen;
  short ind;
  int k = key;

  create_blob_table(dbh);

  if (! (fp = fopen(fname, "r"))) {
    printf("ERROR: cannot open file %s\n", fname);
    return 0;
  }

  /* determine file len */
  fseek(fp, 0, SEEK_END);
  filelen = ftell(fp);
  fseek(fp, 0, 0);

  /* parse */
  if (0>(sth = sqlo_prepare(dbh, stmt)))
    error_exit(dbh, "sqlo_prepare");

  /* allocate the lob locator */
  if (0 > sqlo_alloc_lob_desc(dbh, &loblp))
    error_exit(dbh, "sqlo_alloc_lob_desc");

  /* bind input variables */
  if (SQLO_SUCCESS != 
      (sqlo_bind_by_pos(sth, 1, SQLOT_INT, &k, sizeof(int), NULL, 0)) ||
      (sqlo_bind_by_pos(sth, 2, SQLOT_BLOB, &loblp, 0, &ind, 0))
      ) {
    sqlo_free_lob_desc(dbh, &loblp);
    error_exit(dbh, "sqlo_bind_by_pos");
  }

  /* execute */
  status = sqlo_execute(sth, 1);

  if (SQLO_SUCCESS != status) {
    sqlo_free_lob_desc(dbh, &loblp);
    error_exit(dbh, "sqlo_execute");
  }

  /* write the lob */
  status = sqlo_lob_write_stream(dbh, loblp, filelen, fp);

  if (status < 0) {
    sqlo_free_lob_desc(dbh, &loblp);
    error_exit(dbh, "sqlo_lob_write_stream");
  }

  sqlo_close(sth);
  sqlo_free_lob_desc(dbh, &loblp);
  fclose(fp);

  return (1);
}


/* $Id: ex14.c 286 2004-06-15 10:15:52Z kpoitschke $ */
