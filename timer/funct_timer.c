#include "LPC17xx.h"
#include "timer.h"

static void __init_sampling_timer(LPC_TIM_TypeDef *LPC_TIM, IRQn_Type IRQn,  uint32_t timerInterval){
	LPC_TIM->MR0 = timerInterval;
	LPC_TIM->MCR = 0b011; 	//stop reset interrupt
	NVIC_EnableIRQ(IRQn);
	NVIC_SetPriority(IRQn, 4);		/* less priority than buttons */
}

void init_sampling_timer( uint8_t timer_num, uint32_t TimerInterval){
	switch (timer_num) {
		case 0: __init_sampling_timer(LPC_TIM0, TIMER0_IRQn, TimerInterval); break;
		case 1: __init_sampling_timer(LPC_TIM1, TIMER1_IRQn, TimerInterval); break;
		case 2: __init_sampling_timer(LPC_TIM2, TIMER2_IRQn, TimerInterval); break;
		case 3: __init_sampling_timer(LPC_TIM3, TIMER3_IRQn, TimerInterval); break;
	}
}