/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           RIT.h
** Descriptions:        Prototipi per la gestione del Repetitive Interrupt Timer (RIT)
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#ifndef __RIT_H
#define __RIT_H

#include "LPC17xx.h"
#include <stdint.h>

/* * Inizializza il RIT.
 * RITInterval: Valore di confronto (ticks).
 * Formula: RITInterval = (Frequenza_CPU * Tempo_in_secondi).
 * Esempio: Per 50ms a 100MHz -> 100.000.000 * 0.05 = 5.000.000
 */
extern uint32_t init_RIT( uint32_t RITInterval );

/* Abilita il conteggio del RIT */
extern void enable_RIT( void );

/* Disabilita il conteggio del RIT */
extern void disable_RIT( void );

/* Resetta il contatore a 0 */
extern void reset_RIT( void );

/* Handler dell'interruzione (chiamato automaticamente) */
extern void RIT_IRQHandler (void);

#endif /* end __RIT_H */