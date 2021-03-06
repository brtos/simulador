/*
 * terminal_cfg.h
 *
 *  Created on: 28/04/2016
 *      Author: Universidade Federal
 */

#ifndef APP_TERMINAL_CFG_H_
#define APP_TERMINAL_CFG_H_

/************* TERMINAL CONFIG *************************/
#define TERM_INPUT_BUFSIZE 		32
#define UP_KEY_CHAR				(char)-32
#define CHARS_TO_DISCARD		1

/* Supported commands */
/* Must be listed in alphabetical order !!!! */

/*  ------ NAME ------- HELP --- */
#define COMMAND_TABLE(ENTRY) \
    ENTRY(help, "Help Command") \
	ENTRY(runst,"Running stats")	\
	ENTRY(top,"System info")    \
	ENTRY(ver,"System version")

#define HELP_DESCRIPTION         1

#define CUSTOM_PRINTF 		1
#if CUSTOM_PRINTF
#include "printf_lib.h"
#define TERM_PRINT(...)		printf_lib(__VA_ARGS__);
#else
#include "stdio.h"
#define TERM_PRINT(...)		printf(__VA_ARGS__); fflush(stdout);
#endif

/*******************************************************/

#endif /* APP_TERMINAL_CFG_H_ */
