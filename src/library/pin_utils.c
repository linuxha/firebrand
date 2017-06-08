/*
 *  Access the parallel port hardware using linux-2.4 or -2.6 ppdev
 *  method.
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


#include <linux/parport.h>
#include <linux/ppdev.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#include "pin_utils.h"
#include "generic-api.h"
#include "misc.h"

#define DEV  "/dev/parport0"

/* work around built-in inversion */
#define FIX 0x0B

int dev = -1;


/*  
 *  Routine only called once
 */
int pin_init( void)
{
  int arg;

  if( (dev = open( DEV, O_RDWR)) < 0) {
    perror( "couldn't open " DEV);
    return -1;
  }


  if( ioctl( dev, PPEXCL) < 0) {
    perror( "PPEXCL");
    goto err;
  }

  if( ioctl( dev, PPCLAIM) < 0) {
    message( "Couldn't claim exclusive access to parallel port. "
	     " Is something else using it?\n");

    /* TODO: Try and do some diagnostics to find out why.  Meanwhile
     *       just report something that might be useful. */
    message( "Try: \"rmmod lp\" as root.\n");
    goto err;
  }

  /* turn off voltages, quickly */
  clear_pins( P_VDD | P_VPP);

  arg = IEEE1284_MODE_COMPAT;
  if (ioctl (dev, PPSETMODE, &arg)) {
    perror ("PPNEGOT");
    goto err;
  }

  arg = 0;
  if( ioctl( dev, PPDATADIR, &arg) < 0) {
    perror( "PDATADIR");
    goto err;
  }

  
  return 0;

 err:
  close( dev);
  dev = -1;
  return -1;
} /* initialise_hardware */




void set_pins( byte_t mask)
{
  struct ppdev_frob_struct a = {mask, (FIX ^ mask) & mask};

  if( ioctl( dev, PPFCONTROL, &a) < 0)
    perror( "PPFCONTROL");
}


void clear_pins( byte_t mask)
{
  struct ppdev_frob_struct a = {mask, FIX & mask};

  if( ioctl( dev, PPFCONTROL, &a) < 0)
    perror( "PPFCONTROL");
}


/* Set the pins in mask to new_state */
byte_t frob_pins( byte_t new_state, byte_t mask)
{
  byte_t state, old_state;

  if( ioctl( dev, PPRCONTROL, &state) < 0) {
    perror( "PPRCONTROL");
    return -1;
  }

  state ^= FIX;

  old_state = state & mask;

  state  = (state & ~mask) | (new_state & mask);

  state ^= FIX;

  if( ioctl( dev, PPWCONTROL, &state) < 0) {
    perror( "PPWCONTROL");
    return -1;
  }

  return old_state;
}



void pin_write( byte_t value)
{
  if( ioctl( dev, PPWDATA, &value) < 0)
    perror( "PPWDATA");
}



byte_t query_pins( byte_t pins)
{
  byte_t state;

  if( ioctl( dev, PPRCONTROL, &state) < 0) {
    perror( "PPRCONTROL");
    return -1;
  }

  return (FIX ^ state) & pins;
} /* query_pin_state */


byte_t query_status( byte_t pins)
{
  byte_t state;

  if( ioctl( dev, PPRSTATUS, &state) < 0) {
    perror( "PPRSTATUS");
    return -1;
  }

  return state & pins;
} /* query_status */
