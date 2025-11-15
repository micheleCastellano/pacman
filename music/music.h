#ifndef MUSIC_H
#define MUSIC_H


//Default: 1.65
#define SPEEDUP 1.6

#define TIMERSCALER 1

#define SECOND 0x17D7840 * TIMERSCALER


typedef char BOOL;
#define TRUE 1
#define FALSE 0

typedef enum note_durations
{
	time_semibiscroma = (unsigned int)(SECOND * SPEEDUP / 64.0f + 0.5), // 1/128
	time_biscroma = (unsigned int)(SECOND * SPEEDUP / 32.0f + 0.5), // 1/64
	time_semicroma = (unsigned int)(SECOND * SPEEDUP / 16.0f + 0.5), // 1/32
	time_croma = (unsigned int)(SECOND * SPEEDUP / 8.0f + 0.5), // 1/16
	time_semiminima = (unsigned int)(SECOND * SPEEDUP / 4.0f + 0.5), // 1/4
	time_minima = (unsigned int)(SECOND * SPEEDUP / 2.0f + 0.5), // 1/2
	time_semibreve = (unsigned int)(SECOND * SPEEDUP + 0.5), // 1
} NOTE_DURATION;

/* k=1/f'*f/n  k=f/(f'*n) k=25MHz/(f'*45) */

typedef enum frequencies
{

	c3 = 4240,	// 131Hz
	cd3 = 3996, // 139Hz
	d3 = 3779,	// 147Hz
	de3 = 3561,	// 156Hz
	e3 = 3367,	// 165Hz
	f3 = 3175,	// 175Hz
	fg3 = 3003,	// 185Hz
	g3 = 2834,	// 196Hz
	ga3 = 2670, // 208Hz
	a3 = 2525,	// 220Hz
	ab3 = 2384,	// 233Hz
	b3 = 2249,	// 247Hz

	c4 = 2120,	// 262Hz
	cd4 = 2005,	// 277Hz
	d4 = 1890,	// 294Hz
	de4 = 1786,	// 311Hz
	e4 = 1684,	// 330Hz
	f4 = 1592,	// 349Hz
	fg4 = 1501,	// 370Hz
	g4 = 1417,	// 392Hz
	ga4 = 1338,	// 415Hz
	a4 = 1263,	// 440Hz
	ab4 = 1192,	// 466Hz
	b4 = 1125,	// 494Hz

	c5 = 1062, 	// 523Hz
	pause = 0		// DO NOT SOUND
} FREQUENCY;


typedef struct 
{
	FREQUENCY freq;
	NOTE_DURATION duration;
} NOTE;

void playNote(NOTE note);
BOOL isNotePlaying(void);
void music_eating(void);
void music_victory(void);
void music_power(void);

#endif
/* EOF */
