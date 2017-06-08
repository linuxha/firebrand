/*
 * Implementation of isBlank command
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

#include "generic-param.h"

#include "firebrand.h"

/**
 *  Routine to determin if a chip is blank.
 */
#define P(A) param->A

int cmd_isBlank( chip_info_t *ci, parameters_t *param)
{
  chip_request_t req;
  int ret;
  
  req.size = P(length);
  req.offset = P(chip_offset);
  req.use_turbo = 1;
  // req.buffer isn't used

  power_up_chip();
  ret = CHIP_isBlank( ci, &req);
  power_down_chip();

  printf( "Chip is%s blank\n", ret ? "" : " NOT");

  return ret;
}

