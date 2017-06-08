/*
 * API for accessing the parallel port.  Any method must implement these functions.
 *
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

#ifndef __PIN_UTILS_H__
#define __PIN_UTILS_H__

#include <linux/parport.h>

typedef unsigned char byte_t;

/* Our definition of pin numbers */

 /* Control lines we write */
#define P_VPP      PARPORT_CONTROL_STROBE /* pin 1  */
#define P_VDD      PARPORT_CONTROL_INIT   /* pin 16 */
#define P_TRI      PARPORT_CONTROL_AUTOFD /* pin 14 */
#define P_S4       PARPORT_CONTROL_SELECT /* pin 17 */

 /* Control lines we read */
#define P_ReadD    PARPORT_STATUS_ACK     /* pin 10 */
#define P_SrReadD  PARPORT_STATUS_BUSY    /* pin 11 */


/* 
 * These are simple macros that wrapper the frob_pins() command.  The
 * pin_frob() takes longer than the equiv {set,clear}_pins(), but
 * returns previous state.
 */
#define SET_PINS(A)          frob_pins( A, A)
#define CLEAR_PINS(A)        frob_pins( 0, A)

int pin_init( void);

/* Routines for the control pins */
void   set_pins( byte_t);
void   clear_pins( byte_t);
byte_t frob_pins( byte_t, byte_t);
byte_t query_pins( byte_t);

/* Routines for the data lines */
void   pin_write( byte_t);

/* Routines for the status lines */
byte_t query_status( byte_t);

#endif
