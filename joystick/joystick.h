/*********************************************************************************************************
 **--------------File
 *Info---------------------------------------------------------------------------------
 ** File name:           joystick.h
 ** Last modified Date:  2018-12-30
 ** Last Version:        V1.00
 ** Descriptions:        Prototypes of functions included in the lib_joystick,
 *funct_joystick .c files
 ** Correlated files:    lib_joystick.c, funct_joystick.c
 **--------------------------------------------------------------------------------------------------------
 *********************************************************************************************************/
#include "LPC17xx.h"

#ifndef __JOYSTICK_H
#define __JOYSTICK_H

#define JOYSTICK_SEL (LPC_GPIO1->FIOPIN & (1 << 25))
#define JOYSTICK_DOWN (LPC_GPIO1->FIOPIN & (1 << 26))
#define JOYSTICK_LEFT (LPC_GPIO1->FIOPIN & (1 << 27))
#define JOYSTICK_RIGHT (LPC_GPIO1->FIOPIN & (1 << 28))
#define JOYSTICK_UP (LPC_GPIO1->FIOPIN & (1 << 29))

/* lib_joystick */
void joystick_init(void);

#endif /* end __JOYSTICK_H */