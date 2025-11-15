#include "button.h"
#include "LPC17xx.h"

extern int b0_down;



void EINT0_IRQHandler(void) /* INT0														 */
{
  NVIC_DisableIRQ(EINT0_IRQn);       /* disable Button interrupts			 */
  LPC_PINCON->PINSEL4 &= ~(1 << 20); /* GPIO pin selection */
  b0_down = 1;

  LPC_SC->EXTINT &= (1 << 0); /* clear pending interrupt         */
}

void EINT1_IRQHandler(void) /* KEY1														 */
{
//   NVIC_DisableIRQ(EINT1_IRQn);       /* disable Button interrupts			 */
//   LPC_PINCON->PINSEL4 &= ~(1 << 22); /* GPIO pin selection */
//   b1_down = 1;

  LPC_SC->EXTINT &= (1 << 1); /* clear pending interrupt         */
}

void EINT2_IRQHandler(void) /* KEY2														 */
{
//   NVIC_DisableIRQ(EINT2_IRQn);       /* disable Button interrupts			 */
//   LPC_PINCON->PINSEL4 &= ~(1 << 24); /* GPIO pin selection */
//   b2_down = 1;

  LPC_SC->EXTINT &= (1 << 2); /* clear pending interrupt         */
}