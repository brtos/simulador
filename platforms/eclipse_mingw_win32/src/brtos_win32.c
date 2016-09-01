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

extern void stimer_test(void);

int main(void) {

	  /* Inicia as variaveis do BRTOS */
	  BRTOS_Init();

	  stimer_test();

	  /* Instala as tarefas */
	  assert(OSInstallTask(&exec,"Teste 1",STACK_SIZE_DEF,3,&th1) != OK);

	  assert(OSInstallTask(&exec2,"Teste 2",STACK_SIZE_DEF,5,&th2) != OK);

	  assert(OSInstallTask(&exec3,"Teste 3",STACK_SIZE_DEF,10,&th3) != OK);

	  /* Inicia o escalonador do BRTOS */
	  assert(BRTOSStart() != OK);

	  return EXIT_SUCCESS;
}
