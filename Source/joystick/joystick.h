/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           joystick.h
** Descriptions:        Prototipi per la gestione del Joystick (Porta 1)
**--------------------------------------------------------------------------------------------------------      
*********************************************************************************************************/
#ifndef __JOYSTICK_H
#define __JOYSTICK_H

#include "LPC17xx.h"

/* * Inizializza i pin del Joystick come GPIO di Input.
 * Configura P1.25 (Select), P1.26 (Down), P1.27 (Left), P1.28 (Right), P1.29 (Up).
 */
void joystick_init(void);

#endif /* end __JOYSTICK_H */