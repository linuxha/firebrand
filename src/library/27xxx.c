/*
 * Routines specific to the 27xxx series of chips.
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
 *
 * Notes:
 *
 *  Chips that start "27C" are built using CMOS technology, but other
 *  than that they're equivalent to the same-numbered 27xxx series
 *  chip.
 */


/*
 */
#include <unistd.h>

#include "chipinfo.h"

#include "generic.h"
#include "pin_utils.h"
#include "misc.h"


#define TURBO_BURN_TIME  1
#define TURBO_BURN_TRIES 30

/*
 *  Prototype call-back routines
 */
int H_27xxx_is_byte_blank( unsigned long, void *);
int H_27xxx_read_byte( unsigned long, void *);
int H_27xxx_turbo_write_byte( unsigned long, void *);
int H_27xxx_single_shot_write_byte( unsigned long, void *);



/*
 *
 *    I N T E R F A C E   R O U T I N E S
 *
 */

int H_27xxx_write_chip( chip_info_t *ci, chip_request_t *req)
{
  if( req->use_turbo)
    return mseq_iterate( ci->addr_lines, H_27xxx_turbo_write_byte, req);
  else
    return mseq_iterate( ci->addr_lines, H_27xxx_single_shot_write_byte, req);
}

int H_27xxx_read_chip( chip_info_t *ci, chip_request_t *req)
{
  return mseq_iterate( ci->addr_lines, H_27xxx_read_byte, req);
}

int H_27xxx_is_blank( chip_info_t *ci, chip_request_t *req)
{
  return mseq_iterate( ci->addr_lines, H_27xxx_is_byte_blank, req);
}


/*
 *
 *    C A L L B A C K   R O U T I N E S
 *
 * These are called once per byte, in an M-sequence
 */


int H_27xxx_is_byte_blank( unsigned long address, void *user_data)
{
  return (n4014_read_current_byte() != 0xFF);
}


int H_27xxx_read_byte( unsigned long address, void *user_data)
{
  chip_request_t *req = user_data;

  /* If we don't want this byte, just skip it */
  if( (address < req->offset) || (address >= req->offset + req->size))
    return 0;

  req->buffer[address - req->offset] = n4014_read_current_byte();
  
  return 0;
}


int H_27xxx_turbo_write_byte( unsigned long address, void *user_data)
{
  chip_request_t *req = user_data;
  unsigned char read_byte, byte;
  int i;

  /* If we don't want to write this byte, just skip it */
  if( (address < req->offset) || (address >= req->offset + req->size))
    return 0;

  byte = req->buffer [address];
  
  /* Loop whilst trying to set the byte */
  for( i = 0; i < TURBO_BURN_TRIES; i++) {

    /* check this byte. If its what we want, break out of the loop */
    if( (read_byte = n4014_read_current_byte()) == byte)
      break;

    /* Check its still feasible to alter this byte */
    if( ((~read_byte) & byte ) > 0) {
      message( "write_chip_byte: chip needs to be wiped, "
	       "want %02X currently %02X (itr %u) @ 0x%05lx\n",
	       byte, read_byte, i+1, address);
      return 1;
    }

    H_27xxx_single_shot_write_byte( address, req);
  }


  if( read_byte == byte) {

    if( i > 1)
      message( "Took %u attempts to write address 0x%05lx\n", i, address);

    /* Some chip documents recommend writing the data once more */
    H_27xxx_single_shot_write_byte( address, req);
  }
  else
    message( "write_current_byte: write failed, %02X != %02X "
	    "after %u attempts (@ 0x%05lx)\n", 
	    read_byte, byte, i+1, address);

  return (read_byte != byte);
} /* H_27xxx_turbo_write_byte */



int H_27xxx_single_shot_write_byte( unsigned long address, void *user_data)
{
  chip_request_t *req = user_data;
  int burn_time;
  
  /* If we don't want to write this byte, just skip it */
  if( (address < req->offset) || (address >= req->offset + req->size))
    return 0;

  /* Use stated burn time, unless we're using the turbo algorithm */
  burn_time = (req->use_turbo) ? TURBO_BURN_TIME : req->burn_time;

  /* Load the new byte onto data lines */
  set_pins( P_S4);
  clear_pins( P_TRI);
  pin_write( req->buffer [address - req->offset]);
  
  nsleep(20); /* FIXME: what should this value be? */

  /* Turn on VPP, pulse ctrl, then turn off VPP */
  make_scheduler_realtime();

  set_pins( P_VPP);

  nsleep( burn_time * 1000);

  clear_pins( P_S4);
  usleep( 20);
  set_pins( P_S4);
  clear_pins( P_VPP);

  make_scheduler_normal();

  return 0;
} /* 27xxx_single_shot_write_byte */
