#include "./joystick/joystick.h"
#include "GLCD/GLCD.h"
#include "LPC17xx.h"
#include "RIT/RIT.h"
#include "TouchPanel/TouchPanel.h"
#include "button_EXINT/button.h"
#include "pacman/pacman.h"
#include "timer/timer.h"
#include "CAN/CAN.h"

#ifdef SIMULATOR
extern uint8_t ScaleFlag;  // <- ScaleFlag needs to visible in order for the emulator to
                           // find the symbol (can be placed also inside system_LPC17xx.h
                           // but since it is RO, it needs more work)
#endif

int main(void) {
  SystemInit();
	CAN_Init();
  LCD_Initialization();
  // LCD_Clear(Black);
  joystick_init();
  BUTTON_init(); /* BUTTON Initialization              */
  
  init_RIT(0x004C4B40, 1); /* RIT Initialization 50 msec = 0.05*100MH */
  init_timer(0, 25000000 / move_per_second, 2);

	init_pacman();
  enable_RIT();
  enable_timer(0);
	
  // LED_init();    /* LED Initialization                 */

  LPC_SC->PCON |= 0x1; /* power-down	mode */
  LPC_SC->PCON &= ~(0x2);

  LPC_PINCON->PINSEL1 |= (1 << 21);
  LPC_PINCON->PINSEL1 &= ~(1 << 20);
  LPC_GPIO0->FIODIR |= (1 << 26);

  while (1) {
    __ASM("wfi");
  }
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
