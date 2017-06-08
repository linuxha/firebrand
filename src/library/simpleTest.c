/*
 * Simple test for pin-level functionality; a playpen for diagnostics.
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
#include <signal.h>
#include <stdlib.h>
#include <ctype.h>

#include "generic.h"
#include "misc.h"
#include "pin_utils.h"

void intr_handler( int signal);
int call_back( unsigned long address, void *user_data);
void prompt( char *msg);
void display_some( void);

/*
 *
 *  Things to try ...
 *       14 == 27c128,   15 == 27c256 
 *  mseq_iterate( 14, call_back, NULL);
 *
 */

int main( int argc, char *argv[])
{
  int i, addr;

  signal( SIGINT, intr_handler);

  if( initialise( 0) <0)
    return 1;

  //set_addr( 0x555);
  //prompt( "Address now 0x555");

  power_up_chip();

  display_some();

  //mseq_iterate( 14, call_back, NULL);

  //prompt( "Done with mseq");
  
  power_down_chip();

  return 0;
}

void display_some( void)
{
  int addr;
  unsigned char out;

  for( addr=0; addr < 0x100; addr++) {
    set_addr( addr);
    out = n4014_read_current_byte();
    printf( "%02x  ", out);
    if( addr % 16 == 15)
      printf( "\n");
    //printf( "0x%06x: %02x %c\n", addr, out, isprint( out) ? out : '.');
  }
}



void intr_handler( int signal)
{
  fprintf( stderr, "User interrupt detected, shutting down...\n");
  reset_hardware();
  exit(1);
} /* intr_handler */




int call_back( unsigned long address, void *user_data)
{
  printf( "Address 0x%06x\n", address);
  //printf( "Address 0x%06x %02X\n", address, n4014_read_current_byte());

  return 0;
}

void prompt( char *msg) 
{
  printf( "%s\n", msg);
  getchar();
}
