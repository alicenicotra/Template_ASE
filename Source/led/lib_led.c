/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           lib_led.c
** Descriptions:        Inizializzazione Hardware dei LED su P2.0 - P2.7
**--------------------------------------------------------------------------------------------------------      
*********************************************************************************************************/
#include "LPC17xx.h"
#include "led.h"

unsigned char led_value = 0; // Inizializzazione variabile globale

/******************************************************************************
** Function name:       LED_init
** Descriptions:        Configura i pin P2.0 -> P2.7 come Output GPIO
******************************************************************************/
void LED_init(void) {

    /* 1. Configurazione PIN FUNCTION (PINSEL4)
     * PINSEL4 controlla la metà bassa della Porta 2 (P2.0 - P2.15).
     * Ogni pin ha 2 bit. Vogliamo "00" (GPIO) per i primi 8 pin (P2.0-P2.7).
     * 0xFFFF0000 mantiene inalterati i pin dal P2.8 in su, e azzera i primi 8.
     */
    LPC_PINCON->PINSEL4 &= 0xFFFF0000;  
    
    /* 2. Configurazione DIREZIONE (FIODIR)
     * 1 = Output, 0 = Input.
     * Mettiamo a 1 i primi 8 bit (0x000000FF).
     */
    LPC_GPIO2->FIODIR   |= 0x000000FF;  
    
    /* 3. Stato Iniziale (FIOCLR)
     * Spegniamo tutti i LED all'avvio.
     * FIOSET accende, FIOCLR spegne.
     */
    LPC_GPIO2->FIOCLR    = 0x000000FF;  
    
    led_value = 0;
}

/******************************************************************************
** Function name:       LED_deinit
** Descriptions:        Riporta i pin in modalità Input (sicurezza)
******************************************************************************/
void LED_deinit(void) {
    /* Imposta i primi 8 bit a 0 (Input) */
    LPC_GPIO2->FIODIR &= 0xFFFFFF00;  
}