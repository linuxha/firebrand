/*
 * Implementation of write command
 *
 * Copyright 2005 Paul Millar
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "generic-param.h"

#include "firebrand.h"

#define P(A) param->A

int cmd_write( chip_info_t *ci, parameters_t *param)
{
  chip_request_t req;
  unsigned long file_len, read_len, req_len;
  int fh, r;

  req.size = P(length);
  req.offset = P(chip_offset);

  if( (fh = open( P(filename), O_RDONLY)) == -1) {
    perror( P(filename) );
    return 1;
  }

  file_len = lseek( fh, 0L, SEEK_END);

  if( P(file_offset) > file_len) {
    fprintf( stderr, "File offset (%lu) is greater than file length (%lu)\n",
	     P(file_offset), file_len);
    close( fh);
    return 1;
  }
  
  if( (file_len - P(file_offset)) < req.size) {

    fprintf( stderr, "%s: file is %lu bytes long, which is too short "
	     "for read request at offset %lu (%lu bytes)\n",
	     P(filename), file_len, P(file_offset), req.size);

    fprintf( stderr, "%s: remaining space (%lu bytes) will be padded out with 0.\n",
	       P(filename), req.size - (file_len - P(file_offset)));
  }

  if( lseek( fh, P(file_offset), SEEK_SET) == -1) {
    fprintf( stderr, "%s: unable to seek offset %lu\n",
	     P(filename), P(file_offset));
    close( fh);
    return 1;
  }

  req.buffer = (unsigned char *) malloc( req.size);

  req_len = file_len - P(file_offset);
  if( req_len > req.size)
    req_len = req.size;

  /* Read in to buffer */
  memset( req.buffer, 0, req.size);
  read_len = read( fh, req.buffer, req_len);
  close( fh);

  if( read_len != req_len) {
    fprintf( stderr, "error reading full length of file. Requested %lu bytes, got %lu bytes\n", req_len, read_len);
    free( req.buffer);
    return 1;
  }
  
  fprintf( stderr, "Writing to chip %s.\n", ci->name);

  power_up_chip();
  r = CHIP_write( ci, &req);
  power_down_chip();

  free( req.buffer);

  return r;
} /* cmd_write */


