/*
 *  Definitions used within the supported chip database.
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

#ifndef __CHIPINFO_H__
#define __CHIPINFO_H__


/* 
 *  If one wants to read from or write to a chip, fill out this
 *  request information.
 */

typedef struct {
  unsigned char *buffer;  /* Memory for reading to or writing from */
  unsigned long size;     /* Size to read/write. The size of buffer */
  unsigned long offset;   /* Chip offset. buffer[0] == chip [offset] */
  int burn_time;          /* Duration of burn in microseconds */
  int use_turbo;          /* Should we use the turbo algo? */
} chip_request_t;



/*
 *  Structures needed for the chip database.
 */

#define GENERIC_FUNCTION(A) int (* A)( struct _chip_info_t *, chip_request_t *)

typedef struct _chip_info_t {
  int   id;                     /* Unique identifying number */
  char  *name;                  /* Name of particular chip */
  char  *description;           /* Describing this chip */
  int   cident;                 /* chip identifying code */
  int   loc;                    /* Location of the chip on PCB */
  int   addr_lines;             /* Number of address lines */
  int   switches;               /* How to configure the hardware */

  struct {                      /* API for chip operations */
    GENERIC_FUNCTION( isBlank);
    GENERIC_FUNCTION( makeBlank);
    GENERIC_FUNCTION( read);
    GENERIC_FUNCTION( write);
    GENERIC_FUNCTION( test);
    GENERIC_FUNCTION( id);
  } op;

} chip_info_t;



/*
 *   Definitions used within the database.
 */

/* Locations:
 *   1 = ZIF (full)
 *   2 = ZIF (lower)
 *   3 = 
 *   4 =
 *   5 = 
 *   6 = 
 */

/* DIP switch settings (off by default) */
#define SW_DIP1  0x0001
#define SW_DIP2  0x0002
#define SW_DIP3  0x0004
#define SW_DIP4  0x0008
#define SW_DIP5  0x0010
#define SW_DIP6  0x0020
#define SW_DIP7  0x0040
#define SW_DIP8  0x0080
#define SW_DIP9  0x0100
#define SW_DIP10 0x0200
#define SW_DIP11 0x0400
#define SW_DIP12 0x0800

/* Programming voltage */
#define SW_VPP_12V 0x0000
#define SW_VPP_21V 0x1000

/* Other jumpers, which I haven't thought of a good name for yet */
#define SW_DIP3_A15 0x0000
#define SW_DIP3_S4  0x2000
#define SW_PIN1_VPP 0x0000
#define SW_PIN1_A18 0x4000


/* Some short-hand */
#define CHIP_read(A,B)    A->op.read( A, B)
#define CHIP_write(A,B)   A->op.write( A, B)
#define CHIP_isBlank(A,B) A->op.isBlank( A, B)

/* some variables with data defined elsewhere */
extern chip_info_t chip_info [];

/* and some useful functions */
chip_info_t *searchchipbyname( char *);

#endif
