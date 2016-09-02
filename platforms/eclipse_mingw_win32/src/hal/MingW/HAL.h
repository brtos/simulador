/**
* \file HAL.h
* \brief BRTOS Hardware Abstraction Layer defines and inline assembly
*
* This file contain the defines and inline assembly that are processor dependant.
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
*                                     OS HAL Header for Windows port
*
*
*   Author:   Gustavo Weber Denardin
*   Revision: 1.0
*   Date:     20/03/2009
*
*   Authors:  Carlos Henrique Barriquelo e Gustavo Weber Denardin
*   Revision: 1.2
*   Date:     01/10/2010
*
*********************************************************************************************************/

#ifndef OS_HAL_H
#define OS_HAL_H

#include "OS_types.h"
#include <Windows.h>

/// Supported processors
#define X86				99

/// Define the used processor
#define PROCESSOR 		X86

/// Define the CPU type
#define OS_CPU_TYPE 			INT32U

#define STACK_MARK  			1

/// Define if the optimized scheduler will be used
#define OPTIMIZED_SCHEDULER 	0

/// Define if nesting interrupt is active
#define NESTING_INT 			1

#define TASK_WITH_PARAMETERS 	0

/// Define if its necessary to save status register / interrupt info
#define OS_SR_SAVE_VAR

/// Define stack growth direction
#define STACK_GROWTH 	1            /// 1 -> down; 0-> up

/// Define CPU Stack Pointer Size
#define SP_SIZE 32


extern INT8U  iNesting;
extern INT32U SPvalue;


#define INTERRUPT_SWC				( 0UL )
#define INTERRUPT_TICK				( 1UL )


////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Port Defines                                /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

/// Defines the change context command of the choosen processor
#define ChangeContext()			GeraInterrupcaoSimulada( INTERRUPT_SWC )

#define OS_INT_ENTER()			iNesting++;
#define OS_INT_EXIT()			if(iNesting > 0) iNesting--;


/* Critical section handling. */
void EnterCritical( void );
void ExitCritical( void );


/// Defines the disable interrupts command
#define UserEnterCritical() EnterCritical();
/// Defines the enable interrupts command
#define UserExitCritical() ExitCritical();
  
/// Defines the disable interrupts command
#define OSEnterCritical() UserEnterCritical()
/// Defines the enable interrupts command
#define OSExitCritical() UserExitCritical()

/// Defines the low power command
#define OS_Wait


/// Defines the tick timer interrupt handler code
#define TICKTIMER_INT_HANDLER

#define NUMBER_MIN_OF_STACKED_BYTES (8)

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Functions Prototypes                        /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

#if (TASK_WITH_PARAMETERS == 1)
  void CreateVirtualStack(void(*FctPtr)(void*), INT16U NUMBER_OF_STACKED_BYTES, void *parameters);
#else
  void CreateVirtualStack(void(*FctPtr)(void), INT16U NUMBER_OF_STACKED_BYTES);
#endif

/*****************************************************************************************//**
* \fn void TickTimerSetup(void)
* \brief Tick timer clock setup
* \return NONE
*********************************************************************************************/
void TickTimerSetup(void);

/*****************************************************************************************//**
* \fn void OSRTCSetup(void)
* \brief Real time clock setup
* \return NONE
*********************************************************************************************/
void OSRTCSetup(void);


#if (OPTIMIZED_SCHEDULER == 1)
#define Optimezed_Scheduler()
#endif



void BTOSStartFirstTask(void);

/*
 * Sinaliza que uma interrupcao simulada aconteceu.
*/
void GeraInterrupcaoSimulada( uint32_t InterruptNumber );

/*
 * Instala um "interrupt handler". O numero deve ser unico e maior do que 2
 * (pois 0 e 1 sao usados pelo sistema) e menor do que 32.
 *
 * O "handler" deve retornar um valor maior que 0 (TRUE) para verificar se ncessita-se
 * realizar a troca de contexto
 */
void ConfiguraInterruptHandler( uint32_t InterruptNumber, uint32_t (*Handler)( void ) );



#endif
