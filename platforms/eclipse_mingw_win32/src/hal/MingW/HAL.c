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
/* Obs.: este codigo para simular o BRTOS no Windows foi baseado no simulador para Windows do sistema FreeRTOS,
 * disponivel em: http://www.freertos.org/FreeRTOS-Windows-Simulator-Emulator-for-Visual-Studio-and-Eclipse-MingW.html
 * */
#include "BRTOS.h"
#include "stdint.h"
#include "stdio.h"

#ifdef __GNUC__
	#include "mmsystem.h"
#else
	#pragma comment(lib, "winmm.lib")
#endif

  /* Cria uma thread de alta prioridade para simular o Tick Timer. */
  static DWORD WINAPI TimerSimulado( LPVOID lpParameter );

  /* Cria uma thread de alta prioridade para simular a UART. */
  static DWORD WINAPI UARTSimulada( LPVOID Parameter );

  /*
   * Processa todas as interrupcoes simuladas - cada bit em
   * PendingInterrupts representa uma interrupcao.
   */
  static void ProcessaInterrupcoesSimuladas( void );

  /* "Interrupt handlers" usados pelo BRTOS. */
  static uint32_t SwitchContext( void );
  static uint32_t TickTimer( void );

/*-----------------------------------------------------------*/

  /* O simulador para Windows utiliza a biblioteca de threads do Windows para criar as tarefas e realizar
   * a troca de contexto. Cada tarefa é uma thread. */
  typedef struct
  {
  	void *Thread; /* Ponteiro para a thread que representa a tarefa. */
  } ThreadState;

  /* Variavel usada para simular interrupcoes.  Cada bi representa uma interrupcao. */
  static volatile uint32_t PendingInterrupts = 0UL;

  #define MAX_INTERRUPTS	3 //(sizeof(PendingInterrupts)*8)

  /* Evento usado para avisar a thread que simula as interrupcoes (e tem prioridade mais alta) de que uma interrupcao aconteceu. */
  static void *InterruptEvent = NULL;

  /* Mutex ussado para proteger o acesso das variaveis usadas nas interrupcoes simuladas das tarefas. */
  static void *InterruptEventMutex = NULL;

  /* Vetor de inrerrupcoes simuladas. */
  static uint32_t (*IsrHandler[ MAX_INTERRUPTS ])( void ) = { 0 };

  #define TICK_PERIOD_MS  	(1000/configTICK_RATE_HZ)

  INT32U SPvalue;

  static void usleep(__int64 usec)
  {
      HANDLE timer;
      LARGE_INTEGER ft;

      /* Convert to 100 nanosecond interval,
       * negative value indicates relative time
       */
      ft.QuadPart = -(10*usec);

      timer = CreateWaitableTimer(NULL, TRUE, NULL);
      SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0);
      WaitForSingleObject(timer, INFINITE);
      CloseHandle(timer);
  }

  /*-----------------------------------------------------------*/
  /* Executado no fim da simulacao para retornar o timer com a
   * resolucao anterior
   */
  static BOOL WINAPI EndSimulation( DWORD dwCtrlType )
  {
  	TIMECAPS TimeCaps;

  	( void ) dwCtrlType;

  	if( timeGetDevCaps( &TimeCaps, sizeof( TimeCaps ) ) == MMSYSERR_NOERROR )
  	{
  		timeEndPeriod( TimeCaps.wPeriodMin );
  	}

  	return OK;
  }

  static __int64 increase_timer_resolution(void)
  {
	  	TIMECAPS tc;
	  	__int64 res;

		/* Aumenta para maxima resolucao do timer. */
	  	if(timeGetDevCaps( &tc, sizeof( TIMECAPS ) ) == MMSYSERR_NOERROR )
	  	{
	  		res = tc.wPeriodMin;
	  	}else
	  	{
	  		res = 20;
	  	}
  		timeBeginPeriod(res);
  		SetConsoleCtrlHandler( EndSimulation, TRUE );
  		return res;
  }

  /* Simulacao da interrupcao do Tick Timer*/
  static DWORD WINAPI TimerSimulado( LPVOID Parameter )
  {
	    uint32_t tick_period_ms = (TICK_PERIOD_MS==0)?1:TICK_PERIOD_MS;
	    __int64 res, freq = 0, time_1 = 0, time_2 = 0, elapsed_time_us,
	    		avg_tick_time = 0;

	  	/* Previne aviso do compilador. */
	  	( void ) Parameter;

	  	QueryPerformanceFrequency((LARGE_INTEGER *) &freq);

	  	res=increase_timer_resolution();
	  	if( tick_period_ms < res ) tick_period_ms = res;
	  	QueryPerformanceCounter((LARGE_INTEGER *) &time_1);

	  	for( ;; )
	  	{
	  		/* Gera uma interrupcao simulada do Timer */
	  	    QueryPerformanceCounter((LARGE_INTEGER *) &time_2);
	  	    	elapsed_time_us = (time_2-time_1);
	  	  	  	elapsed_time_us = elapsed_time_us*(1000000UL)/freq;
	  	  	    usleep(tick_period_ms*1000-elapsed_time_us);
	  		QueryPerformanceCounter((LARGE_INTEGER *) &time_1);

	  		GeraInterrupcaoSimulada(INTERRUPT_TICK);

	  		elapsed_time_us = (time_1-time_2);
	  		elapsed_time_us = elapsed_time_us*(1000000UL)/freq;
	  		avg_tick_time+=elapsed_time_us;

	  		if(OSGetTickCount()%1000 == 0)
	  		{
	  			avg_tick_time = avg_tick_time/1000;
	  			//printf("%u\r\n",(uint32_t)avg_tick_time);
	  			avg_tick_time = 0;
	  		}
	  	}

	  	#ifdef __GNUC__
	  		return 0;
	  	#endif
  }
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/
/* Simulacao da interrupcao da UART */
#define BAUDRATE  	  9600
#define CHARTIME_MS   (10000/BAUDRATE)
#include "../drivers/drivers.h"
#include "stdio.h"
#include "conio.h"

static DWORD WINAPI UARTSimulada( LPVOID Parameter )
{

   	/* Just to prevent compiler warnings. */
   	( void ) Parameter;

   	for( ;; )
   	{

   		char c;

   		static HANDLE stdinHandle;
   	    // Get the IO handles
   	    stdinHandle = GetStdHandle(STD_INPUT_HANDLE);

   	    while( 1 )
   	    {
   	        switch( WaitForSingleObject( stdinHandle, 1000 ) )
   	        {
				case( WAIT_TIMEOUT ):
					break; // return from this function to allow thread to terminate
				case( WAIT_OBJECT_0 ):
					if( _kbhit() ) // _kbhit() always returns immediately
					{
					   c = _getch();
					   goto get_char;
					}
					else // some sort of other events , we need to clear it from the queue
					{
						// clear events
						INPUT_RECORD r[512];
						DWORD read;
						ReadConsoleInput( stdinHandle, r, 512, &read );
					}
					break;
				case( WAIT_FAILED ):
					break;
				case( WAIT_ABANDONED ):
					break;
				default:
					break;
			}
   	    }

   	    get_char:

   		WaitForSingleObject( InterruptEventMutex, INFINITE );

   		Sleep((CHARTIME_MS > 0)? CHARTIME_MS:1);
   		/* store received char */
		do{
	   		extern char UART_RX_buffer;
	   		UART_RX_buffer = c;
		}while(0);

   		/* The UART has received a char, generate the simulated event. */
   		PendingInterrupts |= ( 1 << INTERRUPT_UART );

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
		pThreadState = ( ThreadState * )((OS_CPU_TYPE)&STACK[iStackAddress] + (OS_CPU_TYPE)NUMBER_MIN_OF_STACKED_BYTES);


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
 /* Nao utilizda, pois no simulador o timer é simulador por uma thread com prioridade mais alta.
 */
void TickTimerSetup(void){}


////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      OS RTC Setup                                /////
/////                                                  /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/* Nao utilizado */
void OSRTCSetup(void){}

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

		/* Cria e inicia a thread que simula a UART */
		Handle = CreateThread( NULL, 0, UARTSimulada, NULL, CREATE_SUSPENDED, NULL );
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
