/*
 * Implementation of read command
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


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "generic-param.h"

#include "firebrand.h"

/**
 *  Routine to determin if a chip is blank.
 */
#define P(A) param->A

int cmd_read( chip_info_t *ci, parameters_t *param)
{
  chip_request_t req;
  int fh;
  
  req.size = P(length);
  req.offset = P(chip_offset);
  req.buffer = (unsigned char *) malloc( P(length));
  req.use_turbo = 1;

  power_up_chip();
  CHIP_read( ci, &req);
  power_down_chip();

  fh = open( P(filename), O_WRONLY | O_CREAT | O_TRUNC | O_APPEND, 0644);
  if( fh == -1) {
    perror( "save");
    free( req.buffer);
    return 1;
  }

  lseek( fh, P(file_offset), SEEK_SET);
  write( fh, req.buffer, req.size);
  close( fh);

  free( req.buffer);

  return 0;
}

