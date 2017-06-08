/*
 * Simple utility that counts the number of bits that are 1 and 0.
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
#include <unistd.h>
#include <fcntl.h>


#define BUFFER_SIZE 1024

int main( int argc, char *argv[])
{
  int i, j, k, bytes, fh, count1, count0;
  char buffer [BUFFER_SIZE];

  for( i = 1; i < argc; i++) {

    fh = open( argv[i], O_RDONLY);
    if( fh == -1) {
      perror( argv[i]);
      continue;
    }

    count1 = count0 = 0;
    while( (bytes = read( fh, buffer, BUFFER_SIZE)) > 0) {

      for( j = 0; j < bytes; j++) {
	for(k = 0; k < 8; k++)
	  if( buffer[j] & (1<<k))
	    count1++;
	  else
	    count0++;	
      }
    }
    close(fh);

    printf( "%s:\t%u 0s (%.3g%%)\t%u 1s (%.3g%%)\n", argv[i],
	    count0, 100.0*count0/((float)count0+count1),
	    count1, 100.0*count1/((float)count0+count1));
  }

  return 0;
}

