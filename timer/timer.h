/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           timer.h
** Descriptions:        Prototypes of functions included in the lib_timer, funct_timer, IRQ_timer .c files
** Correlated files:    lib_timer.c, funct_timer.c, IRQ_timer.c
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "LPC17xx.h"

#ifndef __TIMER_H
#define __TIMER_H

/* init_timer.c */
void init_timer(uint8_t timer_num, uint32_t timerInterval, uint8_t priority);
void enable_timer(uint8_t timer_num);
void disable_timer(uint8_t timer_num);
void reset_timer(uint8_t timer_num);
/* IRQ_timer.c */
void TIMER0_IRQHandler(void);
void TIMER1_IRQHandler(void);
/* funct_timer.c */
void init_sampling_timer(uint8_t timer_num, uint32_t TimerInterval);

#endif /* end __TIMER_H */
/*****************************************************************************
**                            End Of File
******************************************************************************/
