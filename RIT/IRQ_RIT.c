/*********************************************************************************************************
**--------------File
*Info---------------------------------------------------------------------------------
** File name:           IRQ_RIT.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    RIT.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "../GLCD/GLCD.h"
#include "../joystick/joystick.h"
#include "../led/led.h"
#include "../music/music.h"
#include "../pacman/pacman.h"
#include "../timer/timer.h"
#include "LPC17xx.h"
#include "RIT.h"

extern direction_t new_dir;
extern uint8_t state;
int b0_down = 0;
extern uint8_t state;
extern uint8_t music;

// beat 1/4 = 1.65/4 seconds
#define RIT_SEMIMINIMA 8
#define RIT_MINIMA 16
#define RIT_INTERA 32

#define UPTICKS 1

/******************************************************************************
** Function name:		RIT_IRQHandler
** Descriptions:		REPETITIVE INTERRUPT TIMER handler
** parameters:			None
** Returned value:		None
******************************************************************************/
void RIT_IRQHandler(void) {
  static unsigned int j_up = 0, j_down = 0, j_left = 0, j_right = 0;

  if (JOYSTICK_UP == 0) {
    if (++j_up == 1)
      new_dir = UP;
  } else
    j_up = 0;

  if (JOYSTICK_DOWN == 0) {
    if (++j_down == 1)
      new_dir = DOWN;
  } else
    j_down = 0;

  if (JOYSTICK_RIGHT == 0) {
    if (++j_right == 1)
      new_dir = RIGHT;
  } else
    j_right = 0;

  if (JOYSTICK_LEFT == 0) {
    if (++j_left == 1)
      new_dir = LEFT;
  } else
    j_left = 0;

  /* button management */
  if (b0_down >= 1) {
    if ((LPC_GPIO2->FIOPIN & (1 << 10)) == 0) { /* KEY0 pressed */
      if (b0_down == 2) {
        if (state < 3)
          play_or_pause();
      }
      b0_down++;
    } else { /* button released */
      b0_down = 0;
      NVIC_EnableIRQ(EINT0_IRQn);       /* enable Button interrupts			*/
      LPC_PINCON->PINSEL4 |= (1 << 20); /* External interrupt 0 pin selection */
    }
  }

  switch (music) {
    case 1:
      music_eating();
      break;
    case 2:
      music_power();
      break;
    case 3:
      music_victory();
      break;
    default:
      break;
  }

  LPC_RIT->RICTRL |= 0x1; /* clear interrupt flag */
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
