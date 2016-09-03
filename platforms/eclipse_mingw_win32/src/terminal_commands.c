/* The License
 * 
 * Copyright (c) 2015 Universidade Federal de Santa Maria
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.

*/
/*
 * terminal_commands.c
 *
 */
#include "BRTOS.h"
#include "terminal.h"
#include "stdio.h"
#include "stddef.h"

#define printf_terminal(...)  TERM_PRINT(__VA_ARGS__);
#define SPRINTF(a,...)		  snprintf_lib(a,256,__VA_ARGS__)


void terminal_newline(void)
{
	printf_terminal("\n\r");
}

// BRTOS version Command
CMD_FUNC(ver)
{
  printf_terminal("%s", (CHAR8*)version);
  return NULL;
}

// TOP Command (similar to the linux command)
#include "OSInfo.h"
char big_buffer[1024];
CMD_FUNC(top)
{
  OSCPULoad(big_buffer);
  printf_terminal(big_buffer);

  OSUptimeInfo(big_buffer);
  printf_terminal(big_buffer);

  OSAvailableMemory(big_buffer);
  printf_terminal(big_buffer);

  OSTaskList(big_buffer);
  printf_terminal(big_buffer);

  return NULL;
}

CMD_FUNC(runst)
{
#if (COMPUTES_TASK_LOAD == 1)
  OSRuntimeStats(big_buffer);
  printf_terminal(big_buffer);
#endif
  return NULL;
}
