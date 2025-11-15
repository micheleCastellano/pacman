#include "../CAN/CAN.h"
#include "../GLCD/GLCD.h"
#include "../TouchPanel/TouchPanel.h"
#include "../led/led.h"
#include "../pacman/pacman.h"
#include "LPC17xx.h"
#include "timer.h"

extern direction_t dir, new_dir;
extern uint8_t time, lives, state, mode;
extern uint16_t score;

uint8_t timer10seconds, timer3seconds;

uint16_t SinTable[45] = /* ���ұ�                       */
    {
        410, 467, 523, 576, 627, 673, 714, 749, 778,
        799, 813, 819, 817, 807, 789, 764, 732, 694,
        650, 602, 550, 495, 438, 381, 324, 270, 217,
        169, 125, 87, 55, 30, 12, 2, 0, 6,
        20, 41, 70, 105, 146, 193, 243, 297, 353};

void TIMER0_IRQHandler(void) {
  static unsigned int count = 0;
  static uint8_t tmp = move_per_second, enemy_delay = 3, level = 3;

  count++;

  if (state < 2) {
    if (mode == Frightned) {
      if (--timer10seconds == 0) {
        mode = Chase;
      }
    }

    if (state == Enemy_killed) {
      if (--timer3seconds == 0) {
        spawn_enemy();
        state = Play;
      }
    }

    if (new_dir != dir) {
      if (player_move(new_dir) == 1)
        dir = new_dir;
      else
        player_move(dir);
    } else
      player_move(dir);

    if (state == Play) {
			enemy_delay--;
      if (enemy_delay == 0) {
        enemy_move();
				enemy_delay = level;
      }
    }

    if (--tmp == 0) {  // each second
      tmp = move_per_second;
      time--;
      send_data(time, lives, score);

      if (time % 20 == 0)
        level--;  // increse enemy speed

      if (time == 0) {
        game_over();
      } else {
        generate_power_pill(count);
      }
    }
  }

  LPC_TIM0->IR = 0xf; /* clear interrupt flag */
  return;
}

void TIMER1_IRQHandler(void) {
  static int sineticks = 0;
  /* DAC management */
  static int currentValue;
  currentValue = SinTable[sineticks];
  currentValue -= 410;
  currentValue /= 1;
  currentValue += 410;
  LPC_DAC->DACR = currentValue << 6;
  sineticks++;
  if (sineticks == 45) sineticks = 0;

  LPC_TIM1->IR = 0xf; /* clear interrupt flag */
  return;
}

void TIMER2_IRQHandler(void) {
  disable_timer(1);
  disable_timer(2);
  LPC_TIM2->IR = 0xf; /* clear interrupt flag */
  return;
}

void TIMER3_IRQHandler(void) {
  LPC_TIM3->IR = 0xf; /* clear interrupt flag */
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
