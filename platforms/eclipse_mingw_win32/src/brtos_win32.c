/*
 ============================================================================
 Nome       : brtos_win32.c
 Autor      : Carlos H. Barriquello
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

/* BRTOS includes */
#include "BRTOS.h"
#include "tasks.h"

BRTOS_TH th1, th2, th3;

#define STACK_SIZE_DEF    16		/* tamanho de pilha padrão */

int main(void) {

	  /* Inicia as variaveis do BRTOS */
	  BRTOS_Init();

#if STIMER_TEST
	  extern void stimer_test(void);
	  stimer_test();
#endif

	  /* Instala as tarefas */
	  assert(OSInstallTask(&exec,"Teste 1",STACK_SIZE_DEF,3,&th1) == OK);

	  assert(OSInstallTask(&TarefaADC,"Teste driver ADC",STACK_SIZE_DEF,5,&th2) == OK);

//	  assert(OSInstallTask(&TarefaGPIO,"Teste driver GPIO",STACK_SIZE_DEF,10,&th3) == OK);

//	  assert(OSInstallTask(&SerialTask,"Teste driver UART",STACK_SIZE_DEF,4,NULL) == OK);

	  assert(OSInstallTask(&TerminalTask,"Terminal",STACK_SIZE_DEF,6,NULL) == OK);

	  /* Inicia o escalonador do BRTOS */
	  assert(BRTOSStart() == OK);

	  return EXIT_SUCCESS;
}
