/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           button.h
** Descriptions:        Prototipi per la gestione dei pulsanti INT0, KEY1, KEY2
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#ifndef __BUTTON_H
#define __BUTTON_H

#include "LPC17xx.h"

/* * Inizializza i pulsanti come Interrupt Esterni (EINT).
 * Configura P2.10 (INT0), P2.11 (KEY1), P2.12 (KEY2).
 */
void BUTTON_init(void);

/* Handler degli interrupt (chiamati dall'hardware) */
void EINT0_IRQHandler(void);
void EINT1_IRQHandler(void);
void EINT2_IRQHandler(void);

#endif /* end __BUTTON_H */