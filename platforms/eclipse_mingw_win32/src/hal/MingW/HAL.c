/**
* \file HAL.c
* \brief BRTOS Hardware Abstraction Layer Functions.
*
* This file contain the functions that are processor dependent.
*
*
**/

/*********************************************************************************************************
*                                               BRTOS
*                                Brazilian Real-Time Operating System
*                            Acronymous of Basic Real-Time Operating System
*
*                              
*                                  Open Source RTOS under MIT License
*
*
*
*                                   OS HAL Functions for Win32 port
*
*
*   Author:   Carlos H. Barriquello
*   Revision: 1.0
*   Date:     25/03/2016
*
*********************************************************************************************************/
/* Obs.: este port para simular o BRTOS no Windows foi baseado no simulador para Windows do sistema FreeRTOS,
 * disponivel em: http://www.freertos.org/FreeRTOS-Windows-Simulator-Emulator-for-Visual-Studio-and-Eclipse-MingW.html
 * */
#include "BRTOS.h"
#include "stdint.h"

#ifdef __GNUC__
	#include "mmsystem.h"
#else
	#pragma comment(lib, "winmm.lib")
#endif

   /*
   * Cria uma thread de alta prioridade para simular o Tick Timer
   */
  static DWORD WINAPI TimerSimulado( LPVOID lpParameter );

  /*
   * Processa todas as interrupcoes simuladas - cada bit em
   * PendingInterrupts representa uma interrupcao.
   */
  static void ProcessaInterrupcoesSimuladas( void );

  /*
   * "Interrupt handlers" usados pelo BRTOS.
   */
  static uint32_t SwitchContext( void );
  static uint32_t TickTimer( void );

  /*
   * Called when the process exits to let Windows know the high timer resolution
   * is no longer required.
   */
  static BOOL WINAPI EndProcess( DWORD dwCtrlType );

  /*-----------------------------------------------------------*/

  /* O simulador para Windows utiliza a biblioteca de threads do Windows para criar as tarefas e realizar
   * a troca de contexto. Cada tarefa é uma thread. */
  typedef struct
  {
  	void *Thread; /* Ponteiro para a thread que representa a tarefa. */
  } ThreadState;

  /* Variavel usada para simular interrupcoes.  Cada bi representa uma interrupcao. */
  static volatile uint32_t PendingInterrupts = 0UL;

  #define MAX_INTERRUPTS	(sizeof(PendingInterrupts)*8)

  /* Evento usado para avisar a thread que simula as interrupcoes (e tem prioridade mais alta) de que uma interrupcao aconteceu. */
  static void *InterruptEvent = NULL;

  /* Mutex ussado para proteger o acesso das variaveis usadas nas interrupcoes simuladas das tarefas. */
  static void *InterruptEventMutex = NULL;

  /* Vetor de inrerrupcoes simuladas. */
  static uint32_t (*IsrHandler[ MAX_INTERRUPTS ])( void ) = { 0 };


#define TICK_PERIOD_MS  	(1000/configTICK_RATE_HZ)


  INT32U SPvalue;

  /* SImulacao da interrupcao do Tick Timer*/
  static DWORD WINAPI TimerSimulado( LPVOID Parameter )
  {
	  uint32_t MinimumWindowsBlockTime;

	  TIMECAPS TimeCaps;

  	/* Set the timer resolution to the maximum possible. */
  	if(timeGetDevCaps( &TimeCaps, sizeof( TimeCaps ) ) == MMSYSERR_NOERROR )
  	{
  		MinimumWindowsBlockTime = ( uint32_t ) TimeCaps.wPeriodMin;
  		timeBeginPeriod( TimeCaps.wPeriodMin );

  		/* Register an exit handler so the timeBeginPeriod() function can be
  		matched with a timeEndPeriod() when the application exits. */
  		SetConsoleCtrlHandler( EndProcess, TRUE );
  	}
  	else
  	{
  		MinimumWindowsBlockTime = ( uint32_t ) 20;
  	}

  	/* Just to prevent compiler warnings. */
  	( void ) Parameter;

  	for( ;; )
  	{
  		/* Wait until the timer expires and we can access the simulated interrupt
  		variables.  *NOTE* this is not a 'real time' way of generating tick
  		events as the next wake time should be relative to the previous wake
  		time, not the time that Sleep() is called.  It is done this way to
  		prevent overruns in this very non real time simulated/emulated
  		environment. */
  		if( TICK_PERIOD_MS < MinimumWindowsBlockTime )
  		{
  			Sleep( MinimumWindowsBlockTime );
  		}
  		else
  		{
  			Sleep( TICK_PERIOD_MS );
  		}

  		WaitForSingleObject( InterruptEventMutex, INFINITE );

  		/* The timer has expired, generate the simulated tick event. */
  		PendingInterrupts |= ( 1 << INTERRUPT_TICK );

  		/* The interrupt is now pending - notify the simulated interrupt
  		handler thread. */
  		if( iNesting == 0 )
  		{
  			SetEvent( InterruptEvent );
  		}

  		/* Give back the mutex so the simulated interrupt handler unblocks
  		and can	access the interrupt handler variables. */
  		ReleaseMutex( InterruptEventMutex );
  	}

  	#ifdef __GNUC__
  		/* Should never reach here - MingW complains if you leave this line out,
  		MSVC complains if you put it in. */
  		return 0;
  	#endif
  }
  /*-----------------------------------------------------------*/

  /*-----------------------------------------------------------*/

  static BOOL WINAPI EndProcess( DWORD dwCtrlType )
  {
	  TIMECAPS TimeCaps;

  	( void ) dwCtrlType;

  	if( timeGetDevCaps( &TimeCaps, sizeof( TimeCaps ) ) == MMSYSERR_NOERROR )
  	{
  		/* Match the call to timeBeginPeriod( TimeCaps.wPeriodMin ) made when
  		the process started with a timeEndPeriod() as the process exits. */
  		timeEndPeriod( TimeCaps.wPeriodMin );
  	}

  	return OK;
  }
  /*-----------------------------------------------------------*/

#if (TASK_WITH_PARAMETERS == 1)
void CreateVirtualStack(void(*FctPtr)(void*), INT16U NUMBER_OF_STACKED_BYTES, void *parameters)
#else
void CreateVirtualStack(void(*FctPtr)(void), INT16U NUMBER_OF_STACKED_BYTES)
#endif
{

#if (TASK_WITH_PARAMETERS == 0)
	  void *parameters = NULL;
#endif

		extern OS_CPU_TYPE STACK[];

		ThreadState *pThreadState = NULL;

		OS_CPU_TYPE *stk_pt = (OS_CPU_TYPE*)&STACK[iStackAddress + (NUMBER_OF_STACKED_BYTES / sizeof(OS_CPU_TYPE))];

		int8_t *TopOfStack = ( int8_t * ) stk_pt;
		pThreadState = ( ThreadState * ) ( TopOfStack - sizeof( ThreadState ) );


		/* Cria a thread da tarefa. */
 	  	pThreadState->Thread = CreateThread( NULL, 0, ( LPTHREAD_START_ROUTINE ) FctPtr, parameters, CREATE_SUSPENDED, NULL );
 	  	if( pThreadState->Thread == NULL) return;
 	  	SetThreadAffinityMask( pThreadState->Thread, 0x01 );
 	  	SetThreadPriorityBoost( pThreadState->Thread, TRUE );
 	  	SetThreadPriority( pThreadState->Thread, THREAD_PRIORITY_IDLE );

 	  	return;

}

/*-----------------------------------------------------------*/


  ////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////
  /////      OS Tick Timer Setup                         /////
  /////                                                  /////
  ////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////

void TickTimerSetup(void)
{
	/* Nao utilizda, pois no simulador o timer é simulador por uma thread com prioridade mais alta.
	 */
}


////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      OS RTC Setup                                /////
/////                                                  /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

void OSRTCSetup(void)
{  
  /* Nao utilizado */
}

/*-----------------------------------------------------------*/
/* "handler" da interrupcao do timer */
static uint32_t TickTimer(void)
{

  OSIncCounter();
  OS_TICK_HANDLER();
  
  return TRUE;

}

/*-----------------------------------------------------------*/
/* "handler" da interrupcao para troca de contexto */
static uint32_t SwitchContext(void)
{
	return TRUE;
}
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

void BTOSStartFirstTask( void )
{
	uint8_t Success = TRUE;
	void *Handle;
	ThreadState *pThreadState;

	/* Instala os "interrupt handlers" usados pelo BRTOS. */
	ConfiguraInterruptHandler( INTERRUPT_SWC, SwitchContext );
	ConfiguraInterruptHandler( INTERRUPT_TICK, TickTimer );

	/* Cria os eventos e mutexes usados para sincronizar as threads. */
	InterruptEventMutex = CreateMutex( NULL, FALSE, NULL );
	InterruptEvent = CreateEvent( NULL, FALSE, FALSE, NULL );

	if( ( InterruptEventMutex == NULL ) || ( InterruptEvent == NULL ) )
	{
		Success = FALSE;
	}

	/* Coloca a prioridade desta para um valor mais alto. Assim, ela pode simular
	 * as interrupcoes que "interrompem" as tarefas. */
	Handle = GetCurrentThread();
	if( Handle == NULL )
	{
		Success = FALSE;
	}

	if( Success == TRUE )
	{
		if( SetThreadPriority( Handle, THREAD_PRIORITY_NORMAL ) == 0 )
		{
			Success = FALSE;
		}
		SetThreadPriorityBoost( Handle, TRUE );
		SetThreadAffinityMask( Handle, 0x01 );
	}

	if( Success == TRUE )
	{
		/* Cria e inicia a thread que simula o Tick Timer */
		Handle = CreateThread( NULL, 0, TimerSimulado, NULL, CREATE_SUSPENDED, NULL );
		if( Handle != NULL )
		{
			SetThreadPriority( Handle, THREAD_PRIORITY_BELOW_NORMAL );
			SetThreadPriorityBoost( Handle, TRUE );
			SetThreadAffinityMask( Handle, 0x01 );
			ResumeThread( Handle );
		}

		/* Inicia a tarefa de maior prioridade */
		pThreadState = ( ThreadState * ) ( ( size_t * ) SPvalue );
		iNesting = 0;
		ResumeThread( pThreadState->Thread );

		/* Atende aos pedidos de interrupcoes */
		ProcessaInterrupcoesSimuladas();
	}

}
/*-----------------------------------------------------------*/


static void ProcessaInterrupcoesSimuladas( void )
{
	uint32_t SwitchRequired, i;
	ThreadState *pThreadState;
	void *ObjectList[ 2 ];
	CONTEXT Context;

	/* Cria bloco com mutex e evento usado para indicar que uma interrupcao ocorreu.  */
	ObjectList[ 0 ] = InterruptEventMutex;
	ObjectList[ 1 ] = InterruptEvent;

	/* Indica que uma interrupcao do Tick Timer ocorreu. */
	PendingInterrupts |= ( 1 << INTERRUPT_TICK );
	SetEvent( InterruptEvent );


	for(;;)
	{
		WaitForMultipleObjects( sizeof( ObjectList ) / sizeof( void * ), ObjectList, TRUE, INFINITE );

		/* Indica se e necessario realizar a troca de contexto */
		SwitchRequired = FALSE;

		/* Executa cada interrupcao que estiver pendente */
		for( i = 0; i < MAX_INTERRUPTS; i++ )
		{
			/* Interrupcao esta pendente ? */
			if( PendingInterrupts & ( 1UL << i ) )
			{
				/* Tem um "handler" associado ? */
				if( IsrHandler[ i ] != NULL )
				{
					/* Executa-o. */
					if( IsrHandler[ i ]() != FALSE )
					{
						SwitchRequired |= ( 1 << i );
					}
				}

				/* Limpa o bit de interrupcao pendente. */
				PendingInterrupts &= ~( 1UL << i );
			}
		}

		if( SwitchRequired != FALSE )
		{

			/* Seleciona a proxima tarefa. */
			 SelectedTask = OSSchedule();

			/* Verifica se a tarefa selecionada e diferente da atual. */
			if( currentTask != SelectedTask )
			{

				/* Suspende a thread antiga */
				pThreadState = ( ThreadState *) ( ( size_t * ) ContextTask[currentTask].StackPoint );
				SuspendThread( pThreadState->Thread );

				/* Verifica se a thread foi mesmo suspensa. */
				Context.ContextFlags = CONTEXT_INTEGER;
				( void ) GetThreadContext( pThreadState->Thread, &Context );


				currentTask = SelectedTask;

				/* Executa a proxima thread e respectiva tarefa. */
				pThreadState = ( ThreadState * ) ( ( size_t *) ContextTask[currentTask].StackPoint);
				ResumeThread( pThreadState->Thread );
			}
		}

		ReleaseMutex( InterruptEventMutex );
	}
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/

void GeraInterrupcaoSimulada( uint32_t InterruptNumber )
{

	if( ( InterruptNumber < MAX_INTERRUPTS ) && ( InterruptEventMutex != NULL ) )
	{
		WaitForSingleObject( InterruptEventMutex, INFINITE );
		PendingInterrupts |= ( 1 << InterruptNumber );

		/* Se estiver em uma secao critica a interrupcao sera agendada mas nao sera executada. */
		if( iNesting == 0 )
		{
			SetEvent( InterruptEvent );
		}

		ReleaseMutex( InterruptEventMutex );
	}
}


/*-----------------------------------------------------------*/

void ConfiguraInterruptHandler( uint32_t InterruptNumber, uint32_t (*Handler)( void ) )
{
	if( InterruptNumber < MAX_INTERRUPTS )
	{
		if( InterruptEventMutex != NULL )
		{
			WaitForSingleObject( InterruptEventMutex, INFINITE );
			IsrHandler[ InterruptNumber ] = Handler;
			ReleaseMutex( InterruptEventMutex );
		}
		else
		{
			IsrHandler[ InterruptNumber ] = Handler;
		}
	}
}
/*-----------------------------------------------------------*/

void EnterCritical( void )
{

	/* Adquire o mutex para desabilitar as interrupcoes simuladas. */
	WaitForSingleObject( InterruptEventMutex, INFINITE );
	iNesting++;
}
/*-----------------------------------------------------------*/

void ExitCritical( void )
{
	int32_t MutexNeedsReleasing;

	/* Se esta aqui, entao esta com o mutex. */
	MutexNeedsReleasing = TRUE;

	/* Verifica se ocorreram mais interrupcoes */
	if(iNesting>0)
	{
		iNesting--;

		if(iNesting == 0 && PendingInterrupts != 0UL )
		{
			SetEvent( InterruptEvent );

			/* Mutex sera liberado */
			MutexNeedsReleasing = FALSE;
			ReleaseMutex( InterruptEventMutex );
		}
	}


	if( InterruptEventMutex != NULL )
	{
		if( MutexNeedsReleasing == TRUE )
		{
			ReleaseMutex( InterruptEventMutex );
		}
	}
}
/*-----------------------------------------------------------*/
