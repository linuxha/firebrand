/*
 * Support routines for generic chip-related activity.
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
#include <string.h>
#include <unistd.h>
#include <sched.h>

#include "misc.h"

#include "generic.h"
#include "generic-api.h"
#include "pin_utils.h"
#include "m-seq.h"

#define NADDRBITS 18
#define HI_ADDR_BIT 0x20000

/*
 * Generic functions for dealing with chips.  Most other chip-related
 * operations call routines in here.
 *
 * NB: these assume 27cXXX-like chips.
 *
 * NB2: all functions assume chip is powered. Use power_up_chip() and
 *      power_down_chip() functions to do this.
 */

/* Prototypes for "private" functions */
void clk_in_addr_bit( int);


/* Some generic routines */
void power_up_chip( void)
{
  set_pins( P_VDD);
}

void power_down_chip( void)
{
  clear_pins( P_VDD | P_VPP); /* we switch off VPP too, just in case */
}



/*
 *   Turn hardware to a known, safe state.
 */
void reset_hardware()
{
  /* turn off voltages */
  clear_pins( P_VDD | P_VPP);

  set_addr( 0x0);
  pin_write( 0x00);

  set_pins( P_S4);
  clear_pins( P_TRI);
  clear_pins( P_S4);

} /* reset_hardware */


/* Clock in an address bit. Address is left-shifted by one bit, the new
 * bit is bit.
 * Trigger is LOW-to-HIGH edge-triggered on P_CLK
 */
void clk_in_addr_bit( int bit)
{
  int data_pin;

#define P_CLK  0x01  /* bit 0 of data lines */  
#define P_D    0x02  /* bit 1 of data lines */

  data_pin = bit ? P_D : 0x00; /* Store the this bit pattern */

  pin_write( data_pin); /* Make sure we don't accidentally clk */
  set_pins( P_TRI);  

    /* Pulse the P_CLK bit, clocking in the bit */
  pin_write( data_pin | P_CLK);
  pin_write( data_pin);

} /* ckl_in_addr_bit */



/*
 *  Routine to set the 18 address lines to a particular value.  This is
 *  sub-optimal as it doesn't take into account existing address line
 *  contents.
 */
void set_addr( unsigned long addr)
{
  int i;

  /*
   *  we don't care about accidentally CLKing in new addr as 
   *  we're about to set all NADDRBITS (18) address lines
   */
  set_pins( P_TRI);

  for( i = 0 ; i < NADDRBITS; i++) {
    clk_in_addr_bit( addr & HI_ADDR_BIT);
    addr <<= 1;
  }  

  nsleep( 260); /* ensure output stable */
}



/*
 *  Read off whatever byte the current address lines point to.
 *
 * Theory of operation:
 *
 *  First one parallel loads the current byte onto the 4014.  One then
 *  reads off this byte, bit by bit.  The Philips HEF4014B datasheet
 *  defines the following pins
 *
 *  PE = "parallel enable input"
 *  CP = "LOW to HIGH edge-triggered clock input"
 *
 *  When PE is HIGH, CP causes data to be loaded into shift register,
 *       PE is LOW,  CP causes a bit-shift to right.
 *
 *  Pin P_ReadCLK is connected to CP via a NOT gate
 *  Pin P_D is connected to PE.
 *
 *  So, translating above into pin activity, to achieve...
 *   parallel load:  P_D is HIGH, P_ReadCLK is HIGH-LOW transition
 *   clock bit:      P_D is LOW,  P_ReadCLK is HIGH-LOW transition
 *
 *  S4 may be connected to /CE (chip enable), so should be low.
 *  S6 is connected to /OE (output enable), so we need this to be low.
 *      S6 is connected to NOT(P_TRI), so we need P_TRI to be high.
 */
unsigned char n4014_read_current_byte( void)
{
  unsigned char a, mask;

  /* Certain data lines have special meaning */
#define P_SrWriteD 0x01
#define P_SrCLK    0x02
#define P_ReadCLK  0x04

  clear_pins( P_S4);
  set_pins( P_TRI);

  /* Sleep a little while, should be something like
     35
     + (old_state & P_S4) ? 80 : 0  // ChipEnable to output valid delay 
     + (old_state & P_TRI) ? ?? : 0
  */
  nsleep( 100);  // Just in case.


  /* load the data onto the 4014 */
  pin_write( P_SrCLK|P_ReadCLK);  /* set to do parallel load */
  pin_write( P_SrCLK);            /* trigger w/ HIGH-LOW on P_ReadCLK*/
  pin_write( P_SrCLK|P_ReadCLK);
  pin_write(         P_ReadCLK);  /* back to serial read out */

  /* read in the bits: MSB is first */
  for( a=0, mask = 1<<7; mask; mask >>= 1) {

    nsleep( 260); /* allow 4014 to settle */

    if( !query_status(P_ReadD))
      a |= mask;

    /* CLK in new bit */
    pin_write( 0x00);
    pin_write( P_ReadCLK);
  }

  return a;
} /* 4014_read_current_byte */


/* 
 * Generic function for performing an arbirary function at each memory
 * location of a chip. It uses an M-sequence, so the order may be
 * non-intuitive.
 */
int mseq_iterate( int addr_lines, int (*fn)( unsigned long, void *),
		  void *user_data)
{
  int new_bit, chip_size, counter, ret;
  unsigned long address=0;

  chip_size = 1<<addr_lines;

  /* Treat zero address as a special case */
  set_addr( address);
  if( (ret = (*fn)( address, user_data)) != 0)
    return ret;

  /* Loop over all bytes on chip */
  for(counter=0; counter < chip_size; counter++) {

    /* build new address. */
    new_bit = next_mseq_number( addr_lines, &address);

    /* clock in new address bit */
    clk_in_addr_bit( new_bit);

    /* sleep to allow chip to get correct data on output */
    //nsleep(35); // guessed value

    /* do function and check result. Terminate early if there's a problem. */
    if( (ret = (*fn)( address, user_data)) != 0)
      break;
  }

  return ret;
} /* mseq_iterate */
