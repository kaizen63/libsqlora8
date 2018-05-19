/* $Id: ex13b.c 286 2004-06-15 10:15:52Z kpoitschke $ */
#include <stdio.h>
#include <stdlib.h>
#include "examples.h"

int update_blob_table(sqlo_db_handle_t dbh, int key )
{
  char * stmt = 
    "UPDATE T_SQLORA_BLOB SET CDATA = EMPTY_CLOB() WHERE KEY=:b1 "
    "RETURNING CDATA INTO :b2";

  char data[MAX_BLOB_BUFFER_DATA];        /* a data buffer */
  sqlo_lob_desc_t loblp;                  /* the lob locator */
  sqlo_stmt_handle_t sth;
  int status;
  int k = key;

  printf("Update CLOB\n");

  /* fill the data buffer with some characters */
  fillbuf2(data, MAX_BLOB_BUFFER_DATA);

  /* parse the statement */

  if (0>(sth = sqlo_prepare(dbh, stmt)))
    error_exit(dbh, "sqlo_prepare");

  /* alloate the lob descriptor */
  if (0 > sqlo_alloc_lob_desc(dbh, &loblp))
    error_exit(dbh, "sqlo_alloc_lob_desc");

  /* bind input variables. Note: we bind the lob descriptor here */
  if (SQLO_SUCCESS != 
      (sqlo_bind_by_pos(sth, 1, SQLOT_INT, &k, sizeof(int), NULL, 0)) ||
      (sqlo_bind_by_pos(sth, 2, SQLOT_CLOB, &loblp, 0, NULL, 0))
      ) {
    sqlo_free_lob_desc(dbh, &loblp);
    error_exit(dbh, "sqlo_bind_by_pos");
  }

  /* execute the statement */
  status = sqlo_execute(sth, 1);

  if (SQLO_SUCCESS != status) {
    sqlo_free_lob_desc(dbh, &loblp);
    error_exit(dbh, "sqlo_execute");
  }

  /* write the lob */
  status = sqlo_lob_write_buffer(dbh, loblp, MAX_BLOB_BUFFER_DATA, data, 
                                 MAX_BLOB_BUFFER_DATA, SQLO_ONE_PIECE);

  if (status < 0) {
    sqlo_free_lob_desc(dbh, &loblp);
    error_exit(dbh, "sqlo_log_write_buffer");
  }

  sqlo_free_lob_desc(dbh, &loblp);
  sqlo_close(sth);
  
  return (1);
}

void fillbuf2(char * data, int len)
{
  int i;
  for (i = 0; i < len; ++i) {
    data[i] = 'a' + i % 26;
  }
}

/* $Id: ex13b.c 286 2004-06-15 10:15:52Z kpoitschke $ */
