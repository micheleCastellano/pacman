/*----------------------------------------------------------------------------
 * Name:    Can.c
 * Purpose: CAN interface for for LPC17xx with MCB1700
 * Note(s): see also http://www.port.de/engl/canprod/sv_req_form.html
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2009 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

#include <LPC17xx.h> /* LPC17xx definitions */

#include "../GLCD/GLCD.h"
#include "../pacman/pacman.h"
#include "CAN.h" /* LPC17xx CAN adaption layer */

extern uint8_t icr;  // icr and result must be global in order to work with both real and simulated landtiger.
extern uint32_t result;
extern CAN_msg CAN_TxMsg; /* CAN message for sending */
extern CAN_msg CAN_RxMsg; /* CAN message for receiving */

/*----------------------------------------------------------------------------
  CAN interrupt handler
 *----------------------------------------------------------------------------*/
void CAN_IRQHandler(void) {
  static uint8_t old_time = 0, old_lives = 0xff;
  static uint16_t old_score = 0xff;

  uint8_t new_time, new_lives;
  static uint16_t new_score;

  /* check CAN controller 2 */
  icr = 0;
  icr = (LPC_CAN1->ICR | icr) & 0xFF; /* clear interrupts */
  icr = (LPC_CAN2->ICR | icr) & 0xFF; /* clear interrupts */

  if (icr & (1 << 0)) {       /* CAN Controller #2 meassage is received */
    CAN_rdMsg(2, &CAN_RxMsg); /* Read the message */
    LPC_CAN2->CMR = (1 << 2); /* Release receive buffer */

    new_time = CAN_RxMsg.data[0];
    new_lives = CAN_RxMsg.data[1];
    new_score = CAN_RxMsg.data[2] << 8;
    new_score = new_score | CAN_RxMsg.data[3];

    if (new_time != old_time) {
      old_time = new_time;
      print_time(old_time);
    }
    if (new_lives != old_lives) {
      old_lives = new_lives;
      print_lives(old_lives);
    }
    if (new_score != old_score) {
      old_score = new_score;
      print_score(old_score);
    }
  }
  if (icr & (1 << 1)) { /* CAN Controller #2 meassage is transmitted */
    // do nothing
  }
}
