#include "music.h"

#include "../timer/timer.h"
uint8_t music = 1;

NOTE eating[] =
    {
        {cd3, time_croma},
};
NOTE victory[] =
    {
        {c4, time_semibiscroma},
        {d4, time_semibiscroma},
        {e4, time_semibiscroma},
        {f4, time_semibiscroma},
        {g4, time_semibiscroma},
        {f4, time_semibiscroma},
        {e4, time_semibiscroma},
        {d4, time_semibiscroma},
        {c4, time_semibiscroma},
};
NOTE power[] =
    {
        {c4, time_semibiscroma},
        {d4, time_semibiscroma},
        {e4, time_semibiscroma},
        {f4, time_semibiscroma},
        {g4, time_semibiscroma},
};

void playNote(NOTE note) {
  if (note.freq != pause) {
    reset_timer(1);
    init_timer(1, note.freq, 0);
    enable_timer(1);
  }
  reset_timer(2);
  init_timer(2, note.duration, 0);
  enable_timer(2);
}

BOOL isNotePlaying(void) {
  return ((LPC_TIM1->TCR != 0) || (LPC_TIM2->TCR != 0));
}

void music_eating(void) {
  static int currentNote = 0;
  if (!isNotePlaying()) {
    playNote(eating[currentNote++]);
  }

  if (currentNote == (sizeof(eating) / sizeof(eating[0]))) {
    currentNote = 0;
    music = 0;
  }
}
void music_victory(void) {
  static int currentNote = 0;
  if (!isNotePlaying()) {
    playNote(victory[currentNote++]);
  }

  if (currentNote == (sizeof(victory) / sizeof(victory[0]))) {
    currentNote = 0;
    music = 0;
  }
}
void music_power(void) {
  static int currentNote = 0;
  if (!isNotePlaying()) {
    playNote(power[currentNote++]);
  }

  if (currentNote == (sizeof(power) / sizeof(power[0]))) {
    currentNote = 0;
    music = 0;
  }
}