#include "GLCD.h"
#include "LPC17xx.h"

#ifndef __PACMAN_H
#define __PACMAN_H

#define move_per_second 10

typedef enum direction {
  UP,
  DOWN,
  RIGHT,
  LEFT,
} direction_t;

typedef struct position {
  uint16_t row;
  uint16_t col;
} position_t;

enum Mode {
  Chase,
  Frightned
};

enum State {
  Play,
  Enemy_killed,
  Pause,
  Over,
};

void init_pacman(void);
uint8_t player_move(direction_t d);
void generate_power_pill(unsigned int seed);
void print_time(uint8_t time);
void print_lives(uint8_t lives);
void print_score(uint16_t score);

void play_or_pause(void);
void game_over(void);
void enemy_move(void);
void spawn_enemy(void);

#endif /* end __PACMAN_H */
