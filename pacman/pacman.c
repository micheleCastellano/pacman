#include "pacman.h"

#include <stdint.h>
#include <stdio.h> /*for sprintf*/
#include <stdlib.h>

#include "../CAN/CAN.h"
#include "../GLCD/GLCD.h"
#include "../timer/timer.h"
#include "LPC17xx.h"

#define MAX_ROW 40
#define MAX_COL 30
#define BASE_LENGTH 8

#define p__empty 0
#define p_stdpil 1
#define p_powpil 2
#define p_plopen 3
#define p_plclos 4
#define p_cor_hl 5
#define p_cor_hr 6
#define p_cor_ll 7
#define p_cor_lr 8
#define p_wa_hor 9
#define p_wa_ver 10
#define p_echase 11
#define p_efrigh 12

#define init_row_pl 22
#define init_col_pl 14
#define init_row_en 19
#define init_col_en 14

extern uint8_t timer10seconds, timer3seconds;
extern uint8_t music;

uint8_t lives = 1, time = 60, state = Play, pre_state = Pause, mode = Chase, init_enemy = 1;
uint16_t score = 0;
position_t player_pos = {init_row_pl, init_col_pl}, enemy_pos = {init_row_en, init_col_en};
direction_t dir = RIGHT, new_dir = RIGHT;

const uint16_t pattern[13][8][8] = {
    {
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
    },
    {
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, White, White, 0, 0, 0},
        {0, 0, 0, White, White, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
    },
    {
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, White, White, White, White, White, White, 0},
        {0, White, White, White, White, White, White, 0},
        {0, White, White, White, White, White, White, 0},
        {0, White, White, White, White, White, White, 0},
        {0, White, White, White, White, White, White, 0},
        {0, White, White, White, White, White, White, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
    },
    {
        {0, 0, Yellow, Yellow, Yellow, Yellow, Yellow, 0},
        {0, Yellow, Yellow, Yellow, Yellow, Yellow, 0, 0},
        {Yellow, Yellow, Yellow, Yellow, Yellow, 0, 0, 0},
        {Yellow, Yellow, Yellow, Yellow, 0, 0, 0, 0},
        {Yellow, Yellow, Yellow, Yellow, Yellow, 0, 0, 0},
        {Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, 0, 0},
        {0, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, 0},
        {0, 0, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow},
    },
    {
        {0, 0, Yellow, Yellow, Yellow, Yellow, 0, 0},
        {0, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, 0},
        {Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow},
        {Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow},
        {Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow},
        {Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow},
        {0, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, 0},
        {0, 0, Yellow, Yellow, Yellow, Yellow, 0, 0},
    },
    {
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, Blue, Blue, Blue},
        {0, 0, 0, 0, Blue, Blue, Blue, Blue},
        {0, 0, 0, Blue, Blue, Blue, 0, 0},
        {0, 0, 0, Blue, Blue, 0, 0, 0},
        {0, 0, 0, Blue, Blue, 0, 0, 0},
    },
    {
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {Blue, Blue, Blue, 0, 0, 0, 0, 0},
        {Blue, Blue, Blue, Blue, 0, 0, 0, 0},
        {0, 0, Blue, Blue, Blue, 0, 0, 0},
        {0, 0, 0, Blue, Blue, 0, 0, 0},
        {0, 0, 0, Blue, Blue, 0, 0, 0},
    },
    {
        {0, 0, 0, Blue, Blue, 0, 0, 0},
        {0, 0, 0, Blue, Blue, 0, 0, 0},
        {0, 0, 0, 0, Blue, Blue, 0, 0},
        {0, 0, 0, 0, Blue, Blue, Blue, Blue},
        {0, 0, 0, 0, 0, Blue, Blue, Blue},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
    },
    {
        {0, 0, 0, Blue, Blue, 0, 0, 0},
        {0, 0, 0, Blue, Blue, 0, 0, 0},
        {0, 0, Blue, Blue, Blue, 0, 0, 0},
        {Blue, Blue, Blue, Blue, 0, 0, 0, 0},
        {Blue, Blue, Blue, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
    },
    {
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {Blue, Blue, Blue, Blue, Blue, Blue, Blue, Blue},
        {Blue, Blue, Blue, Blue, Blue, Blue, Blue, Blue},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
    },
    {
        {0, 0, 0, Blue, Blue, 0, 0, 0},
        {0, 0, 0, Blue, Blue, 0, 0, 0},
        {0, 0, 0, Blue, Blue, 0, 0, 0},
        {0, 0, 0, Blue, Blue, 0, 0, 0},
        {0, 0, 0, Blue, Blue, 0, 0, 0},
        {0, 0, 0, Blue, Blue, 0, 0, 0},
        {0, 0, 0, Blue, Blue, 0, 0, 0},
        {0, 0, 0, Blue, Blue, 0, 0, 0},
    },
    {
        {0, 0, Red, Red, Red, Red, 0, 0},
        {0, Red, Red, Red, Red, Red, Red, 0},
        {Red, White, White, Red, Red, White, White, Red},
        {Red, White, 0, Red, Red, 0, White, Red},
        {Red, Red, Red, Red, Red, Red, Red, Red},
        {Red, Red, Red, Red, Red, Red, Red, Red},
        {Red, 0, Red, 0, Red, 0, Red, 0},
        {Red, 0, Red, 0, Red, 0, Red, 0},
    },
    {
        {0, 0, Blue, Blue, Blue, Blue, 0, 0},
        {0, Blue, Blue, Blue, Blue, Blue, Blue, 0},
        {Blue, White, White, Blue, Blue, White, White, Blue},
        {Blue, White, 0, Blue, Blue, 0, White, Blue},
        {Blue, Blue, Blue, Blue, Blue, Blue, Blue, Blue},
        {Blue, Blue, Blue, Blue, Blue, Blue, Blue, Blue},
        {Blue, 0, Blue, 0, Blue, 0, Blue, 0},
        {Blue, 0, Blue, 0, Blue, 0, Blue, 0},
    },
    // {
    //     {0, 0, 0, 0, 0, 0, 0, 0},
    //     {0, 0, 0, 0, 0, 0, 0, 0},
    //     {0, 0, 0, 0, 0, 0, 0, 0},
    //     {0, 0, 0, 0, 0, 0, 0, 0},
    //     {0, 0, 0, 0, 0, 0, 0, 0},
    //     {0, 0, 0, 0, 0, 0, 0, 0},
    //     {0, 0, 0, 0, 0, 0, 0, 0},
    //     {0, 0, 0, 0, 0, 0, 0, 0},
    // },
};

uint8_t maze[40][30] = {
    /*   0         1         2         3         4         5         6         7         8         9        10        11        12        13         14        15        16        17       18        19         20        21        22        23       24        25        26        27         28       29   */
    {p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty}, /*0*/
    {p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty}, /*1*/
    {p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty}, /*2*/
    {p_cor_hl, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_cor_hr}, /*3*/
    {p_wa_ver, p_cor_hl, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_cor_hr, p_cor_hl, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_cor_hr, p_wa_ver}, /*4*/
    {p_wa_ver, p_wa_ver, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_wa_ver, p_wa_ver, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_wa_ver, p_wa_ver}, /*5*/
    {p_wa_ver, p_wa_ver, p_stdpil, p_cor_hl, p_wa_hor, p_wa_hor, p_cor_hr, p_stdpil, p_cor_hl, p_wa_hor, p_wa_hor, p_wa_hor, p_cor_hr, p_stdpil, p_wa_ver, p_wa_ver, p_stdpil, p_cor_hl, p_wa_hor, p_wa_hor, p_wa_hor, p_cor_hr, p_stdpil, p_cor_hl, p_wa_hor, p_wa_hor, p_cor_hr, p_stdpil, p_wa_ver, p_wa_ver}, /*6*/
    {p_wa_ver, p_wa_ver, p_stdpil, p_wa_ver, p__empty, p__empty, p_wa_ver, p_stdpil, p_wa_ver, p__empty, p__empty, p__empty, p_wa_ver, p_stdpil, p_wa_ver, p_wa_ver, p_stdpil, p_wa_ver, p__empty, p__empty, p__empty, p_wa_ver, p_stdpil, p_wa_ver, p__empty, p__empty, p_wa_ver, p_stdpil, p_wa_ver, p_wa_ver}, /*7*/
    {p_wa_ver, p_wa_ver, p_stdpil, p_wa_ver, p__empty, p__empty, p_wa_ver, p_stdpil, p_wa_ver, p__empty, p__empty, p__empty, p_wa_ver, p_stdpil, p_wa_ver, p_wa_ver, p_stdpil, p_wa_ver, p__empty, p__empty, p__empty, p_wa_ver, p_stdpil, p_wa_ver, p__empty, p__empty, p_wa_ver, p_stdpil, p_wa_ver, p_wa_ver}, /*8*/
    {p_wa_ver, p_wa_ver, p_stdpil, p_cor_ll, p_wa_hor, p_wa_hor, p_cor_lr, p_stdpil, p_cor_ll, p_wa_hor, p_wa_hor, p_wa_hor, p_cor_lr, p_stdpil, p_cor_ll, p_cor_lr, p_stdpil, p_cor_ll, p_wa_hor, p_wa_hor, p_wa_hor, p_cor_lr, p_stdpil, p_cor_ll, p_wa_hor, p_wa_hor, p_cor_lr, p_stdpil, p_wa_ver, p_wa_ver}, /*9*/
    {p_wa_ver, p_wa_ver, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_wa_ver, p_wa_ver}, /*10*/
    {p_wa_ver, p_wa_ver, p_stdpil, p_cor_hl, p_wa_hor, p_wa_hor, p_cor_hr, p_stdpil, p_cor_hl, p_cor_hr, p_stdpil, p_cor_hl, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_cor_hr, p_stdpil, p_cor_hl, p_cor_hr, p_stdpil, p_cor_hl, p_wa_hor, p_wa_hor, p_cor_hr, p_stdpil, p_wa_ver, p_wa_ver}, /*11*/
    {p_wa_ver, p_wa_ver, p_stdpil, p_cor_ll, p_wa_hor, p_wa_hor, p_cor_lr, p_stdpil, p_wa_ver, p_wa_ver, p_stdpil, p_cor_ll, p_wa_hor, p_wa_hor, p_cor_hr, p_cor_hl, p_wa_hor, p_wa_hor, p_cor_lr, p_stdpil, p_wa_ver, p_wa_ver, p_stdpil, p_cor_ll, p_wa_hor, p_wa_hor, p_cor_lr, p_stdpil, p_wa_ver, p_wa_ver}, /*12*/
    {p_wa_ver, p_wa_ver, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_wa_ver, p_wa_ver, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_wa_ver, p_wa_ver, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_wa_ver, p_wa_ver, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_wa_ver, p_wa_ver}, /*13*/
    {p_wa_ver, p_cor_ll, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_cor_hr, p__empty, p_wa_ver, p_cor_ll, p_wa_hor, p_wa_hor, p_cor_hr, p__empty, p_wa_ver, p_wa_ver, p__empty, p_cor_hl, p_wa_hor, p_wa_hor, p_cor_lr, p_wa_ver, p__empty, p_cor_hl, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_cor_lr, p_wa_ver}, /*14*/
    {p_cor_ll, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_cor_hr, p_wa_ver, p_stdpil, p_wa_ver, p_cor_hl, p_wa_hor, p_wa_hor, p_cor_lr, p__empty, p_cor_ll, p_cor_lr, p__empty, p_cor_ll, p_wa_hor, p_wa_hor, p_cor_hr, p_wa_ver, p_stdpil, p_wa_ver, p_cor_hl, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_cor_lr}, /*15*/
    {p__empty, p__empty, p__empty, p__empty, p__empty, p_wa_ver, p_wa_ver, p__empty, p_wa_ver, p_wa_ver, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p_wa_ver, p_wa_ver, p__empty, p_wa_ver, p_wa_ver, p__empty, p__empty, p__empty, p__empty, p__empty}, /*16*/
    {p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_cor_lr, p_wa_ver, p_stdpil, p_wa_ver, p_wa_ver, p__empty, p_cor_hl, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_cor_hr, p__empty, p_wa_ver, p_wa_ver, p_stdpil, p_wa_ver, p_cor_ll, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor}, /*17*/
    {p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_cor_lr, p__empty, p_cor_ll, p_cor_lr, p__empty, p_wa_ver, p_cor_hl, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_cor_hr, p_wa_ver, p__empty, p_cor_ll, p_cor_lr, p__empty, p_cor_ll, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor}, /*18*/
    {p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p_stdpil, p__empty, p__empty, p__empty, p_wa_ver, p_wa_ver, p__empty, p__empty, p__empty, p__empty, p_wa_ver, p_wa_ver, p__empty, p__empty, p__empty, p_stdpil, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty}, /*19*/
    {p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_cor_hr, p__empty, p_cor_hl, p_cor_hr, p__empty, p_wa_ver, p_cor_ll, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_cor_lr, p_wa_ver, p__empty, p_cor_hl, p_cor_hr, p__empty, p_cor_hl, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor}, /*20*/
    {p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_cor_hr, p_wa_ver, p_stdpil, p_wa_ver, p_wa_ver, p__empty, p_cor_ll, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_cor_lr, p__empty, p_wa_ver, p_wa_ver, p_stdpil, p_wa_ver, p_cor_hl, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor}, /*21*/
    {p__empty, p__empty, p__empty, p__empty, p__empty, p_wa_ver, p_wa_ver, p__empty, p_wa_ver, p_wa_ver, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p_wa_ver, p_wa_ver, p__empty, p_wa_ver, p_wa_ver, p__empty, p__empty, p__empty, p__empty, p__empty}, /*22*/
    {p_cor_hl, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_cor_lr, p_wa_ver, p_stdpil, p_wa_ver, p_wa_ver, p__empty, p_cor_hl, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_cor_hr, p__empty, p_wa_ver, p_wa_ver, p_stdpil, p_wa_ver, p_cor_ll, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_cor_hr}, /*23*/
    {p_wa_ver, p_cor_hl, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_cor_lr, p__empty, p_cor_ll, p_cor_lr, p__empty, p_cor_ll, p_wa_hor, p_wa_hor, p_cor_hr, p_cor_hl, p_wa_hor, p_wa_hor, p_cor_lr, p__empty, p_cor_ll, p_cor_lr, p__empty, p_cor_ll, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_cor_hr, p_wa_ver}, /*24*/
    {p_wa_ver, p_wa_ver, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_wa_ver, p_wa_ver, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_wa_ver, p_wa_ver}, /*25*/
    {p_wa_ver, p_wa_ver, p_stdpil, p_cor_hl, p_wa_hor, p_wa_hor, p_cor_hr, p_stdpil, p_cor_hl, p_wa_hor, p_wa_hor, p_wa_hor, p_cor_hr, p_stdpil, p_wa_ver, p_wa_ver, p_stdpil, p_cor_hl, p_wa_hor, p_wa_hor, p_wa_hor, p_cor_hr, p_stdpil, p_cor_hl, p_wa_hor, p_wa_hor, p_cor_hr, p_stdpil, p_wa_ver, p_wa_ver}, /*26*/
    {p_wa_ver, p_wa_ver, p_stdpil, p_cor_ll, p_wa_hor, p_cor_hr, p_wa_ver, p_stdpil, p_cor_ll, p_wa_hor, p_wa_hor, p_wa_hor, p_cor_lr, p_stdpil, p_cor_ll, p_cor_lr, p_stdpil, p_cor_ll, p_wa_hor, p_wa_hor, p_wa_hor, p_cor_lr, p_stdpil, p_wa_ver, p_cor_hl, p_wa_hor, p_cor_lr, p_stdpil, p_wa_ver, p_wa_ver}, /*27*/
    {p_wa_ver, p_wa_ver, p_stdpil, p_stdpil, p_stdpil, p_wa_ver, p_wa_ver, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_wa_ver, p_wa_ver, p_stdpil, p_stdpil, p_stdpil, p_wa_ver, p_wa_ver}, /*28*/
    {p_wa_ver, p_cor_ll, p_wa_hor, p_cor_hr, p_stdpil, p_wa_ver, p_wa_ver, p_stdpil, p_cor_hl, p_cor_hr, p_stdpil, p_cor_hl, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_cor_hr, p_stdpil, p_cor_hl, p_cor_hr, p_stdpil, p_wa_ver, p_wa_ver, p_stdpil, p_cor_hl, p_wa_hor, p_cor_lr, p_wa_ver}, /*29*/
    {p_wa_ver, p_cor_hl, p_wa_hor, p_cor_lr, p_stdpil, p_cor_ll, p_cor_lr, p_stdpil, p_wa_ver, p_wa_ver, p_stdpil, p_cor_ll, p_wa_hor, p_wa_hor, p_cor_hr, p_cor_hl, p_wa_hor, p_wa_hor, p_cor_lr, p_stdpil, p_wa_ver, p_wa_ver, p_stdpil, p_cor_ll, p_cor_lr, p_stdpil, p_cor_ll, p_wa_hor, p_cor_hr, p_wa_ver}, /*30*/
    {p_wa_ver, p_wa_ver, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_wa_ver, p_wa_ver, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_wa_ver, p_wa_ver, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_wa_ver, p_wa_ver, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_wa_ver, p_wa_ver}, /*31*/
    {p_wa_ver, p_wa_ver, p_stdpil, p_cor_hl, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_cor_lr, p_cor_ll, p_wa_hor, p_wa_hor, p_cor_hr, p_stdpil, p_wa_ver, p_wa_ver, p_stdpil, p_cor_hl, p_wa_hor, p_wa_hor, p_cor_lr, p_cor_ll, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_cor_hr, p_stdpil, p_wa_ver, p_wa_ver}, /*32*/
    {p_wa_ver, p_wa_ver, p_stdpil, p_cor_ll, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_cor_lr, p_stdpil, p_cor_ll, p_cor_lr, p_stdpil, p_cor_ll, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_cor_lr, p_stdpil, p_wa_ver, p_wa_ver}, /*33*/
    {p_wa_ver, p_wa_ver, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_stdpil, p_wa_ver, p_wa_ver}, /*34*/
    {p_wa_ver, p_cor_ll, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_cor_lr, p_wa_ver}, /*35*/
    {p_cor_ll, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_wa_hor, p_cor_lr}, /*36*/
    {p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty}, /*37*/
    {p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty}, /*38*/
    {p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty, p__empty}, /*39*/
};

static void draw_pattern(uint8_t row, uint8_t col, uint8_t ptn) {
  uint16_t r, c, x = col * BASE_LENGTH, y = row * BASE_LENGTH;
  for (r = 0; r < BASE_LENGTH; r++) {
    for (c = 0; c < BASE_LENGTH; c++) {
      LCD_SetPoint(x + c, y + r, pattern[ptn][r][c]);
    }
  }
}

static void draw_maze(void) {
  uint8_t column, row;
  for (row = 0; row < MAX_ROW; row++) {
    for (column = 0; column < MAX_COL; column++) {
      draw_pattern(row, column, maze[row][column]);
    }
  }
}

void print_lives(uint8_t lives) {
  static uint8_t old_lives = 0;

  uint8_t i, r = 38, c = 1;
  for (i = 0; i < lives; i++) {
    draw_pattern(r, c++, p_plopen);
  }

  if (old_lives > lives) {
    for (i = 0; i < (old_lives - lives); i++) {
      draw_pattern(r, c++, p__empty);
    }
  }

  old_lives = lives;
}

void print_score(uint16_t score) {
  char s[4];
  sprintf(s, "%d", score);
  GUI_Text(150, 16, (uint8_t *)s, White, Black);
}

void print_time(uint8_t time) {
  char s[4];
  if (time < 10)
    sprintf(s, " %d", time);
  else
    sprintf(s, "%d", time);

  GUI_Text(76, 16, (uint8_t *)s, White, Black);
}

void init_pacman(void) {
  draw_maze();

  GUI_Text(144, 0, (uint8_t *)"SCORE", White, Black);
  // print_score();

  GUI_Text(68, 0, (uint8_t *)"TIME", White, Black);
  // print_time();

  // print_lives();
  send_data(time, lives, score);

  draw_pattern(player_pos.row, player_pos.col, p_plclos);
  spawn_enemy();

  play_or_pause();
}

void generate_power_pill(unsigned int seed) {
  static uint8_t tmp = 53, num_powpil = 0;
  uint8_t row, col, found = 0;

  if (num_powpil == 6) return;

  srand(seed);
  row = rand() % MAX_ROW;
  col = rand() % MAX_COL;

  if (maze[row][col] == p_stdpil) {
    maze[row][col] = p_powpil;
    draw_pattern(row, col, p_powpil);
    num_powpil++;
    tmp = tmp - 7;
    return;
  }

  if (time < tmp) {
    tmp = tmp - 7;
    while (found == 0) {
      if (++col >= MAX_COL - 2) {
        col = 2;
        row++;
      }
      if (row >= MAX_ROW - 5) row = 5;

      if (maze[row][col] == p_stdpil) {
        maze[row][col] = p_powpil;
        draw_pattern(row, col, p_powpil);
        num_powpil++;
        found = 1;
      }
    }
  }
}

void play_or_pause(void) {
  static uint8_t backup[3][6];
  uint8_t r, c, from_r = 21, from_c = 12;

  if (state == Pause) {
    uint8_t tmp = state;
    state = pre_state;
    pre_state = tmp;

    for (r = 0; r < 3; r++) {
      for (c = 0; c < 6; c++) {
        draw_pattern(from_r + r, from_c + c, backup[r][c]);
      }
    }

    draw_pattern(player_pos.row, player_pos.col, p_plclos);

    if (state == Play) {
      if (mode == Chase)
        draw_pattern(enemy_pos.row, enemy_pos.col, p_echase);
      else
        draw_pattern(enemy_pos.row, enemy_pos.col, p_efrigh);
    }

  } else {
    pre_state = state;
    state = Pause;
    for (r = 0; r < 3; r++) {
      for (c = 0; c < 6; c++) {
        backup[r][c] = maze[from_r + r][from_c + c];
      }
    }
    GUI_Text(13 * BASE_LENGTH, 21 * BASE_LENGTH, (uint8_t *)"PAUSE", Red, Black);
  }
}

void game_over() {
  state = Over;
  GUI_Text(11 * BASE_LENGTH, 21 * BASE_LENGTH, (uint8_t *)"GAME OVER", Red, Black);
}

static void enemy_killed() {
  state = Enemy_killed;
  draw_pattern(player_pos.row, player_pos.col, p_plclos);
  score += 100;
  send_data(time, lives, score);
  enemy_pos.row = init_row_en;
  enemy_pos.col = init_col_en;
  init_enemy = 1;
  timer3seconds = 3 * move_per_second;
}

static void player_killed() {
  draw_pattern(enemy_pos.row, enemy_pos.col, p_echase);
  lives--;
  send_data(time, lives, score);
  if (lives == 0) {
    game_over();
    return;
  }
  draw_pattern(enemy_pos.row, enemy_pos.col, maze[enemy_pos.row][enemy_pos.col]);
  draw_pattern(player_pos.row, player_pos.col, maze[player_pos.row][player_pos.col]);
  player_pos.row = init_row_pl;
  player_pos.col = init_col_pl;
  enemy_pos.row = init_row_en;
  enemy_pos.col = init_col_en;
  init_enemy = 1;
  play_or_pause();
}

void spawn_enemy() {
  state = Play;
  mode = Chase;
  draw_pattern(enemy_pos.row, enemy_pos.col, p_echase);
}

uint8_t player_move(direction_t d) {
  static const position_t right_tel = {19, 29}, left_tel = {19, 0};
  static uint16_t num_pills = 240, threshold = 1000;
  position_t new;
  uint8_t ret = 0;

  if (state > 1)
    return ret;

  switch (d) {
    case UP:
      new.row = player_pos.row - 1;
      new.col = player_pos.col;
      break;
    case DOWN:
      new.row = player_pos.row + 1;
      new.col = player_pos.col;
      break;
    case RIGHT:
      if (player_pos.col == right_tel.col && player_pos.row == right_tel.row) {
        new.col = left_tel.col;
        new.row = left_tel.row;
      } else {
        new.col = player_pos.col + 1;
        new.row = player_pos.row;
      }
      break;
    case LEFT:
      if (player_pos.col == left_tel.col) {
        new.col = right_tel.col;
        new.row = right_tel.row;
      } else {
        new.col = player_pos.col - 1;
        new.row = player_pos.row;
      }
      break;
  }

  switch (maze[new.row][new.col]) {
    case p_powpil:
      score += 40;
      mode = Frightned;
      timer10seconds = 10 * move_per_second;
      music = 2;
    case p_stdpil:
      score += 10;
      send_data(time, lives, score);
      if (score >= threshold) {
        lives++;
        threshold = threshold + 1000;
        send_data(time, lives, score);
      }
      num_pills--;
      maze[new.row][new.col] = p__empty;
      if (!(music == 2))
        music = 1;
    case p__empty:
      draw_pattern(player_pos.row, player_pos.col, p__empty);
      player_pos.row = new.row;
      player_pos.col = new.col;
      draw_pattern(player_pos.row, player_pos.col, p_plclos);
      ret = 1;
  }

  if (num_pills == 0) {
    state = Over;
    GUI_Text(11 * BASE_LENGTH, 21 * BASE_LENGTH, (uint8_t *)"VICTORY", Red, Black);
    music = 3;
  }

  if (enemy_pos.row == player_pos.row && enemy_pos.col == player_pos.col) {
    if (mode == Chase) {
      player_killed();
    } else {
      enemy_killed();
    }
  }

  return ret;
}

static uint8_t find(direction_t direc, position_t curr, position_t goal, uint8_t depth) {
  uint8_t best = 0xff, tmp;
  position_t enew;

  if (goal.row == curr.row && goal.col == curr.col) {
    return depth;
  }

  if (maze[curr.row][curr.col] > 2) {
    return 0xff;
  }

  if (++depth > 11) {
    return 0xff;
  }

  if (direc != RIGHT) {
    enew.row = curr.row;
    enew.col = curr.col - 1;
    tmp = find(LEFT, enew, goal, depth);
    if (tmp < best)
      best = tmp;
  }

  if (direc != LEFT) {
    enew.row = curr.row;
    enew.col = curr.col + 1;
    tmp = find(RIGHT, enew, goal, depth);
    if (tmp < best)
      best = tmp;
  }

  if (direc != DOWN) {
    enew.row = curr.row - 1;
    enew.col = curr.col;
    tmp = find(UP, enew, goal, depth);
    if (tmp < best)
      best = tmp;
  }

  if (direc != UP) {
    enew.row = curr.row + 1;
    enew.col = curr.col;
    tmp = find(DOWN, enew, goal, depth);
    if (tmp < best)
      best = tmp;
  }

  return best;
}

void enemy_move() {
  uint8_t moved = 0, en_type, row_diff, col_diff;
  position_t egoal, enew;

  if (state != Play) return;

  if (mode == Chase) {
    en_type = p_echase;
    egoal.row = player_pos.row;
    egoal.col = player_pos.col;
  } else {
    en_type = p_efrigh;

    if (player_pos.row > 20)
      egoal.row = 10;
    else
      egoal.row = 24;

    if (player_pos.col > 15)
      egoal.col = 7;
    else
      egoal.col = 22;
  }

  if (init_enemy) {
    enew.row = enemy_pos.row - 3;
    enew.col = enemy_pos.col;
    init_enemy = 0;
    moved = 1;
  }

  row_diff = abs(egoal.row - enemy_pos.row);
  col_diff = abs(egoal.col - enemy_pos.col);

  if (!moved && row_diff < 6 && col_diff < 6) {
    uint8_t i, decision;
    uint8_t costing[4];

    enew.row = enemy_pos.row - 1;
    enew.col = enemy_pos.col;
    costing[UP] = find(UP, enew, egoal, 0);

    enew.row = enemy_pos.row + 1;
    // enew.col = enemy_pos.col;
    costing[DOWN] = find(DOWN, enew, egoal, 0);

    enew.row = enemy_pos.row;
    enew.col = enemy_pos.col - 1;
    costing[LEFT] = find(LEFT, enew, egoal, 0);

    // enew.row = enemy_pos.row;
    enew.col = enemy_pos.col + 1;
    costing[RIGHT] = find(RIGHT, enew, egoal, 0);

    decision = 0;
    for (i = 1; i < 4; i++) {
      if (costing[i] < costing[decision]) {
        decision = i;
      }
    }

    if (costing[decision] < 0xff) {
      moved = 1;
      switch (decision) {
        case UP:
          enew.row = enemy_pos.row - 1;
          enew.col = enemy_pos.col;
          break;
        case DOWN:
          enew.row = enemy_pos.row + 1;
          enew.col = enemy_pos.col;
          break;
        case LEFT:
          enew.row = enemy_pos.row;
          enew.col = enemy_pos.col - 1;
          break;
        case RIGHT:
          enew.row = enemy_pos.row;
          enew.col = enemy_pos.col + 1;
          break;
      }
    }
  }

  if (!moved &&
      egoal.row > enemy_pos.row &&
      maze[enemy_pos.row + 1][enemy_pos.col] < 3) {
    moved = 1;
    enew.row = enemy_pos.row + 1;
    enew.col = enemy_pos.col;
  }

  if (!moved &&
      egoal.row < enemy_pos.row &&
      maze[enemy_pos.row - 1][enemy_pos.col] < 3) {
    moved = 1;
    enew.row = enemy_pos.row - 1;
    enew.col = enemy_pos.col;
  }

  if (!moved &&
      egoal.col > enemy_pos.col &&
      maze[enemy_pos.row][enemy_pos.col + 1] < 3) {
    moved = 1;
    enew.row = enemy_pos.row;
    enew.col = enemy_pos.col + 1;
  }

  if (!moved &&
      egoal.col < enemy_pos.col &&
      maze[enemy_pos.row][enemy_pos.col - 1] < 3) {
    moved = 1;
    enew.row = enemy_pos.row;
    enew.col = enemy_pos.col - 1;
  }

  if (moved) {
    draw_pattern(enemy_pos.row, enemy_pos.col, maze[enemy_pos.row][enemy_pos.col]);
    enemy_pos.row = enew.row;
    enemy_pos.col = enew.col;
    draw_pattern(enemy_pos.row, enemy_pos.col, en_type);

    if (enemy_pos.row == player_pos.row && enemy_pos.col == player_pos.col) {
      if (mode == Chase)
        player_killed();
      else
        enemy_killed();
    }
  }
}
