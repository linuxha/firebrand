/*
 * Implementation of chipInfo command
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


/* private functions */
void print_switch_settings( chip_info_t *info);



/*
 *  Routine to display information about a specific chip.
 */

int cmd_chipInfo( chip_info_t *ci, parameters_t *param)
{

  printf( "Chip: %s (%s)\n\n", ci->name, ci->description);

  print_switch_settings( ci);

  return 0;
}



/*
 *  Pretty-print the switch settings
 */
void print_switch_settings( chip_info_t *info)
{
  char dip[26] = "                         ";
  int i;
  char *bigchip1="     ", *bigchip2="     ";
  char *leftline = "    ";
  char diag3=' ', diag4=' ', diag5=' ', diag6=' ';
  char down3=' ', *right4="  ", *right5="   ", *right6="   ";
  char chip3=' ', chip4=' ', chip5=' ', chip6=' ';

  switch( info->loc) {

  case 1:
    bigchip1 = "XXXXX";
    /* FALL THROUGH */

  case 2:
    bigchip2 = "XXXXX";
    leftline = "----";
    break;

  case 3:
    diag3 = '\\';
    down3 = '|';
    chip3 = 'X';
    break;

  case 4:
    diag4 = '\\';
    right4 = "\\_";
    chip4='X';
    break;
   
  case 5:
    diag5 = '\\';
    right5 = "\\__";
    chip5='X';
    break;

  case 6:
    diag6 = '/';
    right6 = ".__";
    chip6='X';
    break;
  }

  for( i = 0; i < 12; i++)
    dip[2*i+1] = (info->switches & (1<<i)) ? '~' : '_';

  printf( "                   _\n");
  printf( "  +----------------V---------------------------------+\n");
  printf( "  |                | +-----+                         |\n");
  printf( "  |                +-|%s|                       . |\n", bigchip1);
  printf( " +--+                |%s|                  +-+  %c |\n",
	  bigchip2, info->switches & SW_VPP_21V ? '!' : '.');
  printf( " +  |                |%s|               %s|%c|  %c |\n",
	  bigchip2, right6, chip6, info->switches & SW_VPP_21V ? '.' : '!');
  printf( " |  |                |%s|              %c   +-+    |\n",
	  bigchip2, diag6);
  printf( " |  |                |%s|%sPlace Chip    +-+    |\n",
	  bigchip2, leftline);
  printf( " |  |                |%s|         Here %c   |%c|    |\n",
	  bigchip2, diag5, chip5);
  printf( " |  |                +-----+            %c%c %s|%c|    |\n",
	  diag3, diag4, right5, chip5);
  printf( " |  |                                    %c%c   |%c|    |\n",
	  diag3, diag4, chip5);
  printf( " |  |       1 2 3 4 5 6 7 8 9 101112      %c%c  +-+    |\n",
	  down3, diag4);
  printf( " +  |   . +-------------------------+On  +-+%s+-+    |\n",
	  right4);
  printf( " +--+   %c |%s|    |%c|  |%c|    |\n",
	  info->switches & SW_PIN1_A18 ? '!' : '.', dip, chip3, chip4);
  printf( "  |     %c +-------------------------+Off +-+  +-+    |\n",
	  info->switches & SW_PIN1_A18 ? '.' : '!');
  printf( "  |                                                  |\n");
  printf( "  |             .%c.%c.                                |\n",
	  info->switches & SW_DIP3_S4 ? ' ' : '_',
	  info->switches & SW_DIP3_S4 ? '_' : ' ');
  printf( "  +--------------------------------------------------+\n");
  
} /* print_switch_settings */

