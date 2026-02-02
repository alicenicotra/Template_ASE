/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           timer.h
** Descriptions:        Prototipi delle funzioni per la gestione dei Timer (0-3) su LPC17xx
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#ifndef __TIMER_H
#define __TIMER_H
#include "LPC17xx.h"

/* * init_timer
 * Configura il timer specificato.
 * timer_num:     0, 1, 2, 3 (Numero del timer)
 * Prescaler:     Valore per dividere la frequenza di clock (PCLK). 
 * Contatore incrementa ogni (PR + 1) cicli di PCLK.
 * MatchReg:      0, 1, 2, 3 (Quale registro di confronto usare)
 * SRImatchReg:   Configurazione azione su match (Stop, Reset, Interrupt).
 * Bit 0 = Interrupt, Bit 1 = Reset, Bit 2 = Stop.
 * TimerInterval: Valore target del conteggio (Match Value).
 */
extern uint32_t init_timer( uint8_t timer_num, uint32_t Prescaler, uint8_t MatchReg, uint8_t SRImatchReg, uint32_t TimerInterval );

/* Abilita il conteggio del timer */
extern void enable_timer( uint8_t timer_num );

/* Disabilita il conteggio del timer (pausa) */
extern void disable_timer( uint8_t timer_num );

/* Resetta il contatore del timer a zero */
extern void reset_timer( uint8_t timer_num );

/* Handler delle interruzioni (chiamati automaticamente dall'hardware) */
extern void TIMER0_IRQHandler (void);
extern void TIMER1_IRQHandler (void);
extern void TIMER2_IRQHandler (void);
extern void TIMER3_IRQHandler (void);

#endif /* end __TIMER_H */