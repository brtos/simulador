/*
 ============================================================================
 Nome       : brtos_win32.c
 Autor      : Carlos H. Barriquello
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/* BRTOS includes */
#include "BRTOS.h"
#include "tasks.h"

BRTOS_TH th1, th2, th3;

#define STACK_SIZE_DEF    16		/* tamanho de pilha padrão */

#if 0
#include <assert.h>
#define ASSERT(x) assert(x)
#else
#define ASSERT(x) if(!(x)) while(1){}
#endif

int main(void) {

	  /* Inicia as variaveis do BRTOS */
	  BRTOS_Init();

#if STIMER_TEST
	  extern void stimer_test(void);
	  stimer_test();
#endif

	  /* Instala as tarefas */
	  ASSERT(OSInstallTask(&TarefaExemplo,"Tarefa de exemplo",STACK_SIZE_DEF,31,&th1) == OK);

	  ASSERT(OSInstallTask(&TarefaADC,"Teste driver ADC",STACK_SIZE_DEF,5,&th1) == OK);

//	  ASSERT(OSInstallTask(&TarefaGPIO,"Teste driver GPIO",STACK_SIZE_DEF,10,&th2) == OK);

//	  ASSERT(OSInstallTask(&SerialTask,"Teste driver UART",STACK_SIZE_DEF,4,&th3) == OK);

	  ASSERT(OSInstallTask(&TerminalTask,"Terminal",STACK_SIZE_DEF,6,NULL) == OK);

	  /* Inicia o escalonador do BRTOS */
	  ASSERT(BRTOSStart() == OK);

	  return EXIT_SUCCESS;
}
