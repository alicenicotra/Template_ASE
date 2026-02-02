/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           lib_joystick.c
** Descriptions:        Funzioni atomiche di inizializzazione Joystick
** Correlated files:    joystick.h
**--------------------------------------------------------------------------------------------------------      
*********************************************************************************************************/

#include "LPC17xx.h"
#include "joystick.h"

/******************************************************************************
** Function name:       joystick_init
**
** Descriptions:        Configura i pin del Joystick come INPUT GPIO.
** Pin coinvolti: P1.25, P1.26, P1.27, P1.28, P1.29
**
** parameters:          None
** Returned value:      None
**
******************************************************************************/
void joystick_init(void) {
    
    /* * NOTA SUI REGISTRI PINSEL (Pin Function Select):
     * Ogni pin ha 2 bit dedicati in PINSEL per scegliere la funzione (00 = GPIO).
     * PINSEL3 gestisce la seconda metà della Porta 1 (P1.16 a P1.31).
     *
     * Calcolo dello shift per PINSEL3:
     * (NumeroPin - 16) * 2
     * Es. P1.25 -> (25 - 16) * 2 = 18. Quindi bit 18 e 19.
     */

    /* --- JOYSTICK SELECT (P1.25) --- */
    LPC_PINCON->PINSEL3 &= ~(3 << 18);  // Azzera bit 18 e 19 -> Imposta funzione 00 (GPIO)
    LPC_GPIO1->FIODIR   &= ~(1 << 25);  // Azzera bit 25 -> Imposta direzione 0 (Input)

    /* --- JOYSTICK DOWN (P1.26) --- */
    LPC_PINCON->PINSEL3 &= ~(3 << 20);  // Shift 20: (26-16)*2 = 20. Imposta GPIO.
    LPC_GPIO1->FIODIR   &= ~(1 << 26);  // Input

    /* --- JOYSTICK LEFT (P1.27) --- */
    LPC_PINCON->PINSEL3 &= ~(3 << 22);  // Shift 22: (27-16)*2 = 22. Imposta GPIO.
    LPC_GPIO1->FIODIR   &= ~(1 << 27);  // Input

    /* --- JOYSTICK RIGHT (P1.28) --- */
    LPC_PINCON->PINSEL3 &= ~(3 << 24);  // Shift 24: (28-16)*2 = 24. Imposta GPIO.
    LPC_GPIO1->FIODIR   &= ~(1 << 28);  // Input

    /* --- JOYSTICK UP (P1.29) --- */
    LPC_PINCON->PINSEL3 &= ~(3 << 26);  // Shift 26: (29-16)*2 = 26. Imposta GPIO.
    LPC_GPIO1->FIODIR   &= ~(1 << 29);  // Input

    /* * NOTA HARDWARE:
     * Non serve attivare resistenze di pull-up/pull-down interne qui 
     * perché sulla scheda LandTiger/LPC1768 il circuito del joystick 
     * è solitamente già provvisto di resistenze fisiche esterne.
     */
}