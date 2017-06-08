/*
 * Implementation of listChips command
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

/*
 *  Routine to list all supported chips
 */

int cmd_listChips( chip_info_t *ci, parameters_t *param)
{
  int i;

  printf( "Support available for:\n");
  for( i = 0; chip_info[i].name; i++) {
    printf( "\t%s\t", chip_info[i].name);
    printf( " [%c%c%c%c%c%c] ",
	    chip_info[i].op.isBlank ? 'b' : ' ',
	    chip_info[i].op.makeBlank ? 'B' : ' ',
	    chip_info[i].op.read ? 'r' : ' ',
	    chip_info[i].op.write ? 'w' : ' ',
	    chip_info[i].op.test ? 't' : ' ',
	    chip_info[i].op.id ? 'i' : ' ');
    printf( "%s\n", chip_info[i].description);
  }

  printf( "(b isBlank, B makeBlank, r Read, w Write, t Test, i Identify)\n");

  return 0;
}

