/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           lib_RIT.c
** Descriptions:        Funzioni di configurazione del RIT (Enable, Disable, Init, Reset)
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "LPC17xx.h"
#include "RIT.h"

/******************************************************************************
** Function name:       enable_RIT
** Descriptions:        Abilita il RIT impostando il bit 3 del registro RICTRL.
******************************************************************************/
void enable_RIT( void )
{
		// LPC_RIT->RICOUNTER = 0;       // azzera contatore all’abilitazione -> permette di non chiamare reset_RIT() quando si vuole partire da zero
		LPC_RIT->RICTRL |= (1<<3);  // Bit 3 = Timer Enable
    return;
}

/******************************************************************************
** Function name:       disable_RIT
** Descriptions:        Disabilita il RIT pulendo il bit 3 del registro RICTRL.
******************************************************************************/
void disable_RIT( void )
{
    LPC_RIT->RICTRL &= ~(1<<3); // Bit 3 = Timer Disable
    return;
}

/******************************************************************************
** Function name:       reset_RIT
** Descriptions:        Azzera il contatore del RIT.
******************************************************************************/
void reset_RIT( void )
{
    LPC_RIT->RICOUNTER = 0;     // Scrive 0 nel contatore corrente
    return;
}

/******************************************************************************
** Function name:       init_RIT
** Descriptions:        Configura il RIT.
** Parameters:          RITInterval: Numero di cicli di clock dopo i quali generare interrupt.
******************************************************************************/
uint32_t init_RIT ( uint32_t RITInterval )
{
    /* 1. Accensione Alimentazione RIT */
    // PCONP: Power Control for Peripherals. Bit 16 abilita il RIT.
    LPC_SC->PCONP |= (1<<16); 
    
    /* 2. Selezione Clock */
    // PCLKSEL1: Seleziona la frequenza del clock per il RIT (bit 26 e 27).
    // 01 = CCLK (Clock CPU, solitamente 100MHz). Nessun divisore.
    LPC_SC->PCLKSEL1 &= ~(3<<26); // Pulisce i bit 26 e 27
    LPC_SC->PCLKSEL1 |=  (1<<26); // Imposta a 1 (CCLK)

    /* 3. Impostazione Valore di Confronto */
    LPC_RIT->RICOMPVAL = RITInterval; 

    /* 4. Configurazione Registro di Controllo (RICTRL) */
    // Bit 1: RITENCLR -> 1 = Pulisce il contatore quando raggiunge il valore di confronto (Reset automatico)
    // Bit 2: RITENBR  -> 1 = Abilita il break per debug
    LPC_RIT->RICTRL = (1<<1) | (1<<2); 

    /* 5. Azzera contatore iniziale */
    LPC_RIT->RICOUNTER = 0; 
    
    /* 6. Abilita interruzione nel NVIC (Nested Vectored Interrupt Controller) */
    NVIC_EnableIRQ(RIT_IRQn);
    
    // Opzionale: Imposta priorità. Il RIT di solito ha priorità media.
    NVIC_SetPriority(RIT_IRQn, 5); 

    return (0);
}