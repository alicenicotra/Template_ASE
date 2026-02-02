/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           lib_timer.c
** Descriptions:        Funzioni atomiche per configurare, avviare e fermare i Timer.
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "LPC17xx.h"
#include "timer.h"

/******************************************************************************
** Function name:       enable_timer
** Descriptions:        Abilita il conteggio del timer impostando il bit 0 del TCR.
******************************************************************************/
void enable_timer( uint8_t timer_num )
{
    if ( timer_num == 0 )      LPC_TIM0->TCR = 1;
    else if ( timer_num == 1 ) LPC_TIM1->TCR = 1;
    else if ( timer_num == 2 ) LPC_TIM2->TCR = 1;
    else                       LPC_TIM3->TCR = 1;
    return;
}

/******************************************************************************
** Function name:       disable_timer
** Descriptions:        Disabilita il timer (pausa) azzerando il bit 0 del TCR.
******************************************************************************/
void disable_timer( uint8_t timer_num )
{
    if ( timer_num == 0 )      LPC_TIM0->TCR = 0;
    else if ( timer_num == 1 ) LPC_TIM1->TCR = 0;
    else if ( timer_num == 2 ) LPC_TIM2->TCR = 0;
    else                       LPC_TIM3->TCR = 0;
    return;
}

/******************************************************************************
** Function name:       reset_timer
** Descriptions:        Resetta il Timer Counter (TC) e il Prescale Counter (PC).
** Il bit 1 del TCR forza il reset.
******************************************************************************/
void reset_timer( uint8_t timer_num )
{
    uint32_t regVal;

    if ( timer_num == 0 ) {
        regVal = LPC_TIM0->TCR;
        regVal |= 0x02;             // Imposta il bit 1 (Reset)
        LPC_TIM0->TCR = regVal;
    }
    else if ( timer_num == 1 ) {
        regVal = LPC_TIM1->TCR;
        regVal |= 0x02;
        LPC_TIM1->TCR = regVal;
    }
    else if ( timer_num == 2 ) {
        regVal = LPC_TIM2->TCR;
        regVal |= 0x02;
        LPC_TIM2->TCR = regVal;
    }
    else {
        regVal = LPC_TIM3->TCR;
        regVal |= 0x02;
        LPC_TIM3->TCR = regVal;
    }
    return;
}

/******************************************************************************
** Function name:       init_timer
** Descriptions:        Inizializza il timer con prescaler, match register e interrupt.
**
** Parametri Dettagliati:
** - SRImatchReg: Controlla cosa succede quando il timer raggiunge il valore Match.
** 1 (001 bin): Interrupt (genera interruzione)
** 2 (010 bin): Reset (il timer riparte da 0)
** 3 (011 bin): Interrupt + Reset (comune per timer periodici)
** 4 (100 bin): Stop (ferma il timer)
******************************************************************************/
uint32_t init_timer ( uint8_t timer_num, uint32_t Prescaler, uint8_t MatchReg, uint8_t SRImatchReg, uint32_t TimerInterval )
{
    /* PUNTATORI AI REGISTRI DEL TIMER
     * Si usano questi puntatori per evitare di riscrivere if/else per ogni timer
     * rendendo il codice più compatto e leggibile. */
    LPC_TIM_TypeDef *TIMx;
    IRQn_Type IRQn_Tx;
    uint32_t priority;

    /* Selezione del timer base */
    switch(timer_num){
        case 0: 
            TIMx = LPC_TIM0; 
            IRQn_Tx = TIMER0_IRQn; 
            priority = 0; // Massima priorità
            break;
        case 1: 
            TIMx = LPC_TIM1; 
            IRQn_Tx = TIMER1_IRQn; 
            priority = 1; 
            break;
        case 2: 
            TIMx = LPC_TIM2; 
            IRQn_Tx = TIMER2_IRQn; 
            priority = 2; 
            break;
        case 3: 
            TIMx = LPC_TIM3; 
            IRQn_Tx = TIMER3_IRQn; 
            priority = 3; 
            break;
        default: return (1); // Errore
    }

    /* 1. Imposta il Prescaler (PR)
     * Il timer incrementa ogni (PR + 1) cicli di clock periferica (PCLK).
     * Se PCLK = 25MHz (default) e PR = 0, conta a 25MHz.
     * Se PR = 24, conta a 1MHz (1 microsecondo per tick). 
		 * T_sec = (MR + 1) * (PR + 1) / F_PCLK
		*/
    TIMx->PR = Prescaler;

    /* 2. Imposta il Match Register (MR) 
     * Carica il valore target nel registro specificato (0-3) */
    switch(MatchReg){
        case 0: TIMx->MR0 = TimerInterval; break;
        case 1: TIMx->MR1 = TimerInterval; break;
        case 2: TIMx->MR2 = TimerInterval; break;
        case 3: TIMx->MR3 = TimerInterval; break;
        default: return(1); // Errore
    }

    /* 3. Configura il Match Control Register (MCR)
     * SRImatchReg viene shiftato nella posizione corretta.
     * Ogni Match Register ha 3 bit di controllo in MCR:
     * MR0 usa bit 0-2, MR1 usa bit 3-5, etc. -> Ecco perché "3 * MatchReg" */
    TIMx->MCR &= ~(0x7 << (3 * MatchReg)); // cancella vecchi bit
		TIMx->MCR |= (SRImatchReg << (3 * MatchReg));


    /* 4. Abilita le interruzioni nel controller NVIC (Nested Vectored Interrupt Controller) */
    NVIC_EnableIRQ(IRQn_Tx);
    NVIC_SetPriority(IRQn_Tx, priority); // Imposta priorità (0 è la più alta)

    return (0);
}