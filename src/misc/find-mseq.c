/*
 *  Cheap and nasty program to find an m-seq generator.
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
 *  A "sequence" is a series of numbers where the next one is calculated
 *  by taking the previous number, shifting the bits by one and setting the
 *  least significate bit to either zero or one (depending on the generator).
 *  Such a sequence will always repeat. An m-sequence is a sequence
 *  which has the maximum length. If we remove "0" as a special case
 *  then an n bit m-sequence will have 2^n-1 numbers: 1 through to
 *  2^n.
 *
 *  An example 3 bit m-sequence is: 1, 2, 5, 3, 7, 6, 4. The sequence
 *  obvious if layed out:
 *
 *            +---- next bit
 *            |
 *    1 = 001 0
 *    2 = 010 1
 *    5 = 101 1   110 <-- generator
 *    3 = 011 1
 *    7 = 111 0
 *    6 = 110 0
 *    4 = 100 1
 *
 *  The current number is ANDed with the generator and the bits are
 *  independently XORed to produce the next bit (see for yourself).
 *
 *  Another example is: 1, 3, 7, 6, 5, 2, 4. ie ...
 *
 *    1 = 001 1
 *    3 = 011 1
 *    7 = 111 0    101
 *    6 = 110 1
 *    5 = 101 0
 *    2 = 010 0
 *    4 = 100 1 
 *
 *
 *  I couldn't find any literature on fast ways of searching for
 *  m-sequences, so this program is really dumb: it just searches all
 *  the generators to find ones that generate m-sequences.  The search
 *  starts with the most significant bit (MSB) and least significant
 *  bit (LSB) set (I'm pretty sure this is right).  The idea is to
 *  make it quick enough that the O(exp(n)) doesn't matter too much.
 *  On my K6-II 450MHz, it finds an m-sequences generator for an 18
 *  bit m-sequence in under 8 seconds.
 *
 *  NB you will have to increase your stack size when running this
 *  program: 8Mb doesn't cut it for > 16 bit m-sequences!
 *  "ulimit -s 16384" should fix it.
 */


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

struct done {
  long long pattern;
  struct done *next, *previous;
} *top [256] [256], *free_list = NULL;

int work_unit( int level, long long pattern);
struct done *search_and_add_pattern( long long);
__inline struct done *getnew( void);
void remove_pattern( long long, struct done *);
void printb( int i, int value);
void printc( int i, int value);


long long mask, gen;

unsigned char *results;
short int *storage;
int counter=0, maxlevel;


int main( int argc, char *argv[])
{
  int n;
  int hash1, hash2;

  if( argc != 2) {
    printf( "Need a integer argument (order of m-sequence)\n");
    return 1;
  }

  for( hash1=0; hash1 < 256; hash1++)
    for( hash2=0; hash2 < 256; hash2++)
      top [hash1] [hash2] = NULL;

  n = atoi( argv[1]);

  results = (unsigned char *) malloc( 1<<n);
  storage = (short int *) malloc( (1<<n) * sizeof( short int));

  mask = (1<<n)-1;
  maxlevel = (1<<n)-2;

  search_and_add_pattern( 1);

  for( gen = (1<<(n-1))+1; gen < (1<<n); gen++) {
    if( work_unit( 0, 1) == 0)
      printf( "0x%llX\n", gen);
  }
  
  return 0;
}


/*
 *  Returns 1 if this tree isn't working, 0 on the return stroke after
 *  finding one solution (all we really need) 
 */
int work_unit( int level, long long pattern)
{
  long long bits;
  int count=0;
  struct done *added;

  if( level == maxlevel)
    return 0;

  /* Generate new pattern */
  for(bits = pattern & gen; bits; bits >>=1)
    if( bits & 1)
      count++;

  pattern <<= 1;
  pattern &= mask;

  if( count & 1)
    pattern |= 1;

  if( (added = search_and_add_pattern( pattern))) {

    if( work_unit( level+1, pattern) == 0) {
      remove_pattern( pattern, added);
      return 0;
    }

    remove_pattern( pattern, added);
  }

  return 1;
}



/* 
 *  Returns NULL if pattern found, struct done * if pattern not found
 *  and added 
 */
struct done *search_and_add_pattern( long long pattern)
{
  struct done *current, *new, *currentnext, *t;
  unsigned char hash1, hash2;

  hash1 = pattern & 0xF00F;
  hash2 = (pattern & 0xFF0) >> 4;

  if( (t = top [hash1] [hash2]) == NULL) {
    top [hash1] [hash2] = new = getnew();
    new->pattern = pattern;
    new->previous = new->next = NULL;
    return new;
  }
  if( t->pattern < pattern) {
    top [hash1] [hash2] = new = getnew();
    new->pattern = pattern;
    new->next = t;
    new->previous = NULL;
    return new;
  }

  for( current = t;
       (currentnext = current->next) && (currentnext->pattern >= pattern);
       current = currentnext);

  if( current->pattern == pattern)
    return NULL;

  new = getnew();
  new->pattern = pattern;
  new->next = currentnext;
  new->previous = current;
  current->next = new;
  if( currentnext)
    currentnext->previous = new;

  return new;
} 


__inline struct done *getnew( void)
{
  struct done *new;

  if( free_list) {
    new = free_list;
    free_list = new->next;
    return new;
  }
  else {
    return (struct done *) malloc( sizeof( struct done));
  }
} /* getnew */


void remove_pattern( long long pattern, struct done *this)
{
  unsigned char hash1, hash2;
  struct done *prev, *next;

  if( (prev = this->previous) == NULL) {
    hash1 = pattern & 0xF00F;
    hash2 = (pattern & 0xFF0) >> 4;

    top [hash1] [hash2] = (next = this->next);

    if( next)
      next->previous = NULL;
  }
  else {
    prev->next = (next = this->next);

    if( next)
      this->next->previous = prev;
  }

  this->next = free_list;
  free_list = this;
}



/*
 *  Print a binary representation. i == 0 -> shortest string else i = length
 *  If value >= 1<<i, then we just go ahead anyway.
 */
void printb( int i, int value)
{
  int j, nbits, tmp;

  /* count number of bits */
  for(tmp = value, nbits=0; tmp; tmp>>=1, nbits++);

  if( nbits == 0) {
    for( j = 1; j < i; j++)
      printf( "0");
    printf( "0\n");
    return;
  }


  for( j = i; j > nbits; j--)
    printf( "0");

  for( j = nbits; j > 0; j--)
    printf( "%c", (1<<(j-1)) & value ? '1':'0');

  printf( "\n");


} /* printb */

