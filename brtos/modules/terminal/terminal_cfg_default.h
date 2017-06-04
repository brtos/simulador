/*
 * terminal_cfg.h
 *
 *  Created on: 28/04/2016
 *  Author: Carlos H. Barriquello
 */

#ifndef TERMINAL_CFG_H_
#define TERMINAL_CFG_H_


/************* TERMINAL CONFIG *************************/
#ifndef TERM_INPUT_BUFSIZE
#define TERM_INPUT_BUFSIZE 		32
#endif

#ifndef UP_KEY_CHAR
#define UP_KEY_CHAR				(char)-32
#endif

#ifndef CHARS_TO_DISCARD
#define CHARS_TO_DISCARD		1
#endif

//** Only for reference. Copy and paste this file to a local folder, and chaneg it according your platform*/

#if 0
/* Supported commands */
/* Must be listed in alphabetical order !!!! */

/*  ------ NAME ------- HELP --- */
#define COMMAND_TABLE(ENTRY) \
	ENTRY(help,"Help Command")     \
	ENTRY(runst,"Running stats")	\
	ENTRY(top,"System info")    \
	ENTRY(ver,"System version")

#define HELP_DESCRIPTION         1

#endif

#ifndef TERM_PRINT
#include "printf_lib.h"
#define CUSTOM_PRINTF    1
#define TERM_PRINT(...)  printf_lib(__VA_ARGS__);
#define SPRINTF(a,...)	 snprintf_lib(a,256,__VA_ARGS__);
#endif


/*******************************************************/

#endif /* APP_TERMINAL_CFG_H_ */
