/*
 * A "database" of different chips and support routines.
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


#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "chipinfo.h"

#include "27xxx.h"


chip_info_t chip_info [] = {
  {1, "27c16", "16kb (addressable as 2kB), UV or OTP EPROM",
      0x00, 2, 11, SW_DIP1 | SW_DIP4 | SW_DIP6 | SW_DIP9,
      {H_27xxx_is_blank, NULL, H_27xxx_read_chip, H_27xxx_write_chip,
           NULL, NULL}},
  {2, "27c32", "32kb (addressable as 4kB), UV or OTP EPROM",
      0x00, 2, 12, SW_DIP1 | SW_DIP4 | SW_DIP6 | SW_DIP9,
      {H_27xxx_is_blank, NULL, H_27xxx_read_chip, H_27xxx_write_chip,
           NULL, NULL}},
  {3, "27c64", "64kb (addressable as 8kB), UV or OTP EPROM",
      0x00, 2, 13, SW_DIP1 | SW_DIP4 | SW_DIP6 | SW_DIP9,
      {H_27xxx_is_blank, NULL, H_27xxx_read_chip, H_27xxx_write_chip,
           NULL, NULL}},
  {4, "27c128", "128kb (addressable as 16kB), UV or OTP EPROM",
      0x00, 2, 14, SW_DIP1 | SW_DIP4 | SW_DIP6 | SW_DIP9,
      {H_27xxx_is_blank, NULL, H_27xxx_read_chip, H_27xxx_write_chip,
           NULL, NULL}},
  {5, "27c256", "256kb (addressable as 32kB), UV or OTP EPROM",
      0x00, 2, 15, SW_DIP1 | SW_DIP5 | SW_DIP6 | SW_DIP8 | SW_DIP9,
      {H_27xxx_is_blank, NULL, H_27xxx_read_chip, H_27xxx_write_chip,
           NULL, NULL}},
  {6, "29c020", "2Mb (addressable as 256kB), 5v-only flash EEPROM",
      0x00, 2, 15, 0, 
      {NULL, NULL, NULL, NULL, NULL, NULL}},
  {7, "29F010", "1Mb (addressable as 128kB) Single Supply Flash EEPROM",
      0x00, 2, 15, 0, 
      {NULL, NULL, NULL, NULL, NULL, NULL}},
  {0, NULL, NULL, 0,0,0, 0, {NULL, NULL, NULL, NULL, NULL, NULL}}
};



/*
 *   Look through our list of chips and finds the one that matches 
 *   the name.  Names are unique, so will return either the chipinfo
 *   of the chip or NULL if no match can be found.
 */
chip_info_t *searchchipbyname( char *name)
{
  chip_info_t *result;

  for( result = chip_info; result->name; result++)
    if( !strcasecmp( result->name, name))
      break;

  return (result->name != NULL) ? result : NULL;
} /* searchchipbyname */




/* Handy routine for setting a request to cover all of a chip */
void fb_all_of_chip( chip_request_t *req, chip_info_t *ci)
{
  req->size = 1 << ci->addr_lines;
  req->offset = 0;
  req->buffer = (unsigned char *) malloc( req->size);
}
