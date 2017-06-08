/*
 * Routines not directly related to driving the parallel port.
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


#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <sched.h>
#include <errno.h>
#include <time.h>
#include <string.h>


#include "pin_utils.h"

#include "misc.h"
#include "generic-api.h"


/*
 */

/* Do all initialisation */
int initialise( int should_drop_privs)
{
  int gid;

  if( pin_init() < 0)
    return -1;

  /* Always drop any effective goup-ID */
  if( (gid = getgid()) != getegid()) {
    if( setegid( gid)) {
      perror( "setegid");
      return -1;
    }
  }

  if( should_drop_privs)
    if( drop_privs() < 0)
      return -1;

  reset_hardware();

  return 0;
}



/* 
 *  Drop root privs. Not currently used as we want accurate timing.
 */
int drop_privs( void)
{
  int uid;

    /* Drop any user ID */

  /* If we're root, nothing to drop */
  if( (uid = getuid()) == 0) {
    fprintf( stderr, "Not dropping privs as you are root.\n");
    return 0;
  }

  /* If our effective UID isn't root, nothing to do */
  if( geteuid() != 0) {
    fprintf( stderr, "(not dropping root privs as we're not privileged)\n");
    return 0;
  }

  if( seteuid( uid)) {
    perror( "seteuid");
    return 1;
  }

  return 0;
} /* drop_privs */



/*
 *  Routine to sleep for (approximately) nano nano-seconds. A
 *  real-time scheduler is used briefly obtain ~1us accuracy and to
 *  prevent a context switch.  If we were already using the real-time
 *  scheduler, then it isn't switched back.
 *
 *   From nanosleep man page:
 *
 *  Bugs:
 *     The current implementation of nanosleep is based on the  normal  kernel
 *     timer  mechanism,  which  has  a  resolution  of  1/HZ s (i.e, 10 ms on
 *     Linux/i386 and  1 ms  on  Linux/Alpha).   Therefore,  nanosleep  pauses
 *     always for at least the specified time, however it can take up to 10 ms
 *     longer than specified until the process becomes runnable again. For the
 *     same  reason,  the value returned in case of a delivered signal in *rem
 *     is usually rounded to the next larger multiple of 1/HZ s.
 *
 *
 *  Old behaviour
 *     In order to support applications requiring  much  more  precise  pauses
 *     (e.g.,  in  order  to  control  some time-critical hardware), nanosleep
 *     would handle pauses of up to 2 ms by busy waiting with microsecond pre-
 *     cision  when  called  from a process scheduled under a real-time policy
 *     like SCHED_FIFO or SCHED_RR.  This special  extension  was  removed  in
 *     kernel  2.5.39,  hence is still present in current 2.4 kernels, but not
 *     in 2.6 kernels.
 *
 */
void nsleep( unsigned long nano)
{
  int oldScheduler;
  struct timespec reg, rem;

  oldScheduler = sched_getscheduler( getpid());

  if( oldScheduler != SCHED_FIFO)
    make_scheduler_realtime();

  reg.tv_sec=0;
  reg.tv_nsec=nano;

  /* sleep */    
  while( nanosleep( &reg, &rem))
    if( errno == EINTR)
      memcpy( &reg, &rem, sizeof( struct timespec));
    else
      perror( "nanosleep");

  if( oldScheduler != SCHED_FIFO)
    make_scheduler_normal();
} /* nsleep */



/*
 * Lets us use the real-time FIFO scheduler. This effectively make's
 * us uninterruptable.  If we don't have effective root privs (because,
 * we dropped them, or never had them), then this silently fails.
 */
void make_scheduler_realtime()
{
  struct sched_param p;
  
  p.sched_priority = 1;
  sched_setscheduler( getpid(), SCHED_FIFO, &p);
}



/*
 * Switches back to the normal scheduler, allowing preemptive
 * scheduling.
 */
void make_scheduler_normal()
{
  struct sched_param p;
  
  p.sched_priority = 0;
  if( sched_setscheduler( getpid(), SCHED_OTHER, &p))
    perror( "sched_setscheduler");
}


/*
 * Wrapper around normal printf
 */
int message( char *fmt, ...)
{
  va_list ap;
  int r;
  
  va_start( ap, fmt);
  r = vfprintf( stderr, fmt, ap);
  va_end( ap);

  return r;
}
