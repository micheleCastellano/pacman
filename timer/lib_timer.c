/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           lib_timer.h
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        atomic functions to be used by higher sw levels
** Correlated files:    lib_timer.c, funct_timer.c, IRQ_timer.c
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "LPC17xx.h"
#include "timer.h"

/******************************************************************************
** Function name:		enable_timer
**
** Descriptions:		Enable timer
**
** parameters:			timer number: either 0, 1, 2, or 3
** Returned value:		0 success, 1 error
**
******************************************************************************/
void enable_timer( uint8_t timer_num )
{
	switch (timer_num) {
		case 0: LPC_TIM0->TCR = 1; break;
		case 1: LPC_TIM1->TCR = 1; break;
		case 2: LPC_TIM2->TCR = 1; break;
		case 3: LPC_TIM3->TCR = 1; break;
		default: break;
	}
	return;
}


/******************************************************************************
** Function name:		disable_timer
**
** Descriptions:		Disable timer
**
** parameters:			timer number: either 0, 1, 2, or 3
** Returned value:		0 success, 1 error
**
******************************************************************************/
void disable_timer( uint8_t timer_num )
{
		switch (timer_num) {
		case 0: LPC_TIM0->TCR = 0; break;
		case 1: LPC_TIM1->TCR = 0; break;
		case 2: LPC_TIM2->TCR = 0; break;
		case 3: LPC_TIM3->TCR = 0; break;
		default: break;
	}
		return;
}


/******************************************************************************
** Function name:		reset_timer
**
** Descriptions:		Reset timer
**
** parameters:			timer number: either 0, 1, 2, or 3
** Returned value:		0 success, 1 error
**
******************************************************************************/
void reset_timer( uint8_t timer_num )
{
  switch (timer_num) {
		case 0: LPC_TIM0->TCR |= 0x02; LPC_TIM0->TCR &= ~0x02; break;
		case 1: LPC_TIM1->TCR |= 0x02; LPC_TIM1->TCR &= ~0x02; break;
		case 2: LPC_TIM2->TCR |= 0x02; LPC_TIM2->TCR &= ~0x02; break;
		case 3: LPC_TIM3->TCR |= 0x02; LPC_TIM3->TCR &= ~0x02; break;
		default: break;
	}
  return;
}

static void __init_timer (LPC_TIM_TypeDef *LPC_TIM, IRQn_Type IRQn,  uint32_t TimerInterval, uint8_t priority){
	LPC_TIM->MR0 = TimerInterval;
	LPC_TIM->MCR = 0b011; 	//stop reset interrupt
	NVIC_EnableIRQ(IRQn);
	NVIC_SetPriority(IRQn, priority);		/* more priority than buttons */
	return;
}


/******************************************************************************
** Function name:		init_timer
**
** Descriptions:		Init timer
**
** parameters:			timer number: either 0, 1, 2, or 3
**									timer interval
**
** Returned value:		0 success, 1 error
**
******************************************************************************/
void init_timer ( uint8_t timer_num, uint32_t TimerInterval, uint8_t priority )
{
	switch (timer_num) {
		case 0:  __init_timer(LPC_TIM0, TIMER0_IRQn, TimerInterval, priority); break;
		case 1:  __init_timer(LPC_TIM1, TIMER1_IRQn, TimerInterval, priority); break;
		case 2:  __init_timer(LPC_TIM2, TIMER2_IRQn, TimerInterval, priority); break;
		case 3:  __init_timer(LPC_TIM3, TIMER3_IRQn, TimerInterval, priority); break;
		default: break;
	}
  return;
}


/******************************************************************************
**                            End Of File
******************************************************************************/
