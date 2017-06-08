/*
 * The main part of the CLI
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



#include <sys/time.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

#include "firebrand.h"

#include "generic-param.h"

#include "cmd_read.h"
#include "cmd_write.h"
#include "cmd_isBlank.h"
#include "cmd_listChips.h"
#include "cmd_chipInfo.h"

typedef int (*generic_cmd_t)( chip_info_t *, parameters_t *);

typedef struct {
  char *name;
  generic_cmd_t cmd;
  int needs_chip;
} cmd_info_t;  

/**
 *  needs_chip values:
 *     0 == doesn't need a chip
 *     1 == needs a chip name, but not that chip to be present
 *     2 == needs a chip to be named and present
 */

cmd_info_t Commands[] = {
  {"read",      cmd_read,      2},
  {"write",     cmd_write,     2},
  {"isBlank",   cmd_isBlank,   2},
  {"listChips", cmd_listChips, 0},
  {"chipInfo",  cmd_chipInfo,  1}
};


typedef struct {
  char *name;
  int needs_param;
} opt_info_t;

opt_info_t Options[] = {
  {"filename",   1},
  {"fileoffset", 1},
  {"offset",     1},
  {"length",     1},
  {"chip",       1},
  {"all",        0},
  {"help",       0},
  {"enable-drop-privs",  0},
  {"disable-drop-privs", 0},
};

#define N_COMMANDS (sizeof( Commands)/sizeof( cmd_info_t))
#define N_OPTIONS  (sizeof( Options)/sizeof( opt_info_t))


/* Private functions */
void display_help( void);
void intr_handler( int);
int validate_params( chip_info_t *, cmd_info_t *, parameters_t *);
int proc_arg_string( char *arg);
void process_arg( int *state, parameters_t *p, chip_info_t **, char *arg);
int can_do_cmd( chip_info_t *, int);
void get_anykey( void);

/* Global variables (tut tut) */
int should_drop_privs=0;

int main( int argc, char *argv[])
{
  parameters_t param = {"chipdump.bin", 0, 0, 0, 1};
  chip_info_t *ci = NULL;
  int i, j;
  int told_user=0, state=0;

  signal( SIGINT, intr_handler);
  
  /* Process user's options */
  for( i = 1; i < argc; i++) {
    
    if( *argv[i] == 0)
      continue;

    if( state < 0)
      state *= -1;
    
    if( state == 0) {
      if( *argv[i] != '-')
	break;

      state = proc_arg_string( argv[i]);
    }

    if( state > 0)
      process_arg( &state, &param, &ci, argv[i]);
  } /* for */

  if( initialise( should_drop_privs))
    return 1;

  /* Process remainder of user's command line arguments as commands */
  for(; (i < argc); i++) {

    for( j=0; j < N_COMMANDS; j++)
      if( !strcmp( Commands[j].name, argv[i]))
	break;

    if( j == N_COMMANDS) {
      printf( "Unknown command %s.\n", argv[i]);
      break;
    }

    if( !can_do_cmd( ci, j)) {
      printf( "Command %s is not supported for chip %s.\n",
	      Commands[j].name, ci->name);
      break;
    }


    if( validate_params( ci, &Commands[j], &param))
      break;

    if( (Commands[j].needs_chip == 2) && !told_user) {
      told_user = 1;
      printf( "Place chip %s in its correct location now, "
	      "press RETURN once ready.\n", ci->name);
      get_anykey();
      printf( "Proceeding...\n");
      
    }
	
    Commands[j].cmd( ci, &param);

  } /* for */

  reset_hardware();

  return 0;
} /* main */


void get_anykey()
{
  getchar();
}


void process_arg( int *state, parameters_t *p, chip_info_t **c, char *arg)
{
  chip_info_t *chip;

  switch( *state) {

  case 1:  // filename
    p->filename = strdup( arg);
    break;

  case 2:  // fileoffset
    p->file_offset = atoi( arg);
    break;
      
  case 3:  // offset
    p->chip_offset = atoi( arg);
    break;

  case 4:  // length
    p->length = atoi( arg);
    break;

  case 5:  // chip	
    if( (chip = searchchipbyname( arg)) == NULL) {
      fprintf( stderr, "Couldn't find a matching chip for \"%s\"\n", arg);
      break;
    }
    *c = chip;
    break;

  case 6:  // all
    p->whole_chip = 1;
    break;

  case 7:  // help
    display_help();
    exit(0);
    break;

  case 8: // enable-drop-privs
    should_drop_privs=1;
    break;

  case 9: // disable-drop-privs
    should_drop_privs=0;
    break;

  default:
    printf( "Unknown state: %d (this is probably a bug)\n", *state);
    break;
  } /* switch */

  *state = 0;
}


/* Process a string starting with '-', returns new state */
int proc_arg_string( char *arg )
{
  int i, state;

  if( *(arg+1) != '-') {
    printf( "Short arguments (%s) are not supported.\n", arg);
    return 0;
  }
	
  for( i=0; i < N_OPTIONS; i++)
    if( !strcmp( Options[i].name, arg+2))
      break;

  if( i == N_OPTIONS) {
    fprintf( stderr, "Unknown option: %s\n", arg);
    return 0;
  }

   /* make state 1-offset as 0 has a special meaning */
  state = i+1;

   /* negate the state, indicating need to read param */
  if( Options[i].needs_param)
    state *= -1;

  return state;
}


/**
 *   The ever helpful help info
 */
void display_help( void)
{
  printf( "firebrand [<option> ...] <command> [<command> ...]\n");
  printf( "Possible options:\n");
  printf( "\t--help                display this info\n");
  printf( "\t--all                 process all of chip\n");
  printf( "\t--chip <string>       chip is <string>\n");
  printf( "\t--length <length>     process <length> bytes\n");
  printf( "\t--offset <number>     offset chip activity by <number> bytes\n");
  printf( "\t--fileoffset <number> offset file activity by <number> bytes\n");
  printf( "\t--filename <file>     use filename <file>\n");
  printf( "\t--enable-drop-privs   drop root privileges\n");
  printf( "\t--disable-drop-privs  don't drop root privileges\n");

  printf( "Possible commands:\n");
  printf( "\tread      Read the chip\n");
  printf( "\twrite     Write the chip\n");
  printf( "\tisBlank   Check if the chip is blank\n");
  printf( "\tlistChips Show a list of supported chips\n");
  printf( "\tchipInfo  Show how to configure the EEPROM board\n");

} /* display_help */



/**
 *  Handle an interrupt (Ctrl-C)
 */
void intr_handler( int signal)
{
  fprintf( stderr, "User interrupt detected, shutting down...\n");
  reset_hardware();
  exit(1);
} /* intr_handler */


/**
 *  Check that parameters are valid.  If not, explanation is displayed
 *  and a non-zero value is returned.
 */
int validate_params( chip_info_t *ci, cmd_info_t *cmd, parameters_t *param)
{
  if( cmd->needs_chip) {

    if( ci == NULL) {
      fprintf( stderr, "No chip selected, see --help\n");
      return 1;
    }

    if( param->whole_chip) {
      param->chip_offset = 0;
      param->length = 1 << ci->addr_lines;
      return 0;
    }

    if( (param->length + param->chip_offset) > (1<<ci->addr_lines)) {
      fprintf( stderr,
	       "Length (%lu) for operation too large for operation "
	       "starting at offset (%lu).\n",
	       param->length, param->chip_offset);
      return 1;
    }
  }

  return 0;
}


/**
 *  Returns if a command is supported by a particular chip
 */
int can_do_cmd( chip_info_t *ci, int cmd)
{

  /* There must be a nicer way of doing this */
  switch( cmd) {
  case 0: 
    return ci->op.read != NULL;
  case 1:
    return ci->op.write != NULL;
  case 2:
    return ci->op.isBlank != NULL;
  }

  return 1;
}
