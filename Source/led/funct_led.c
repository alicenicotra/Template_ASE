/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           funct_led.c
** Descriptions:        Funzioni di alto livello per il controllo dei LED
**--------------------------------------------------------------------------------------------------------      
*********************************************************************************************************/
#include "LPC17xx.h"
#include "led.h"

/* Array costante per mappare l'indice del LED al bit corrispondente.
 * 1UL<<0 = 1, 1UL<<1 = 2, ecc.
 */
const unsigned long led_mask[] = { 1UL<<0, 1UL<<1, 1UL<<2, 1UL<< 3, 1UL<< 4, 1UL<< 5, 1UL<< 6, 1UL<< 7 };

/******************************************************************************
** Function name:       LED_On
** Descriptions:        Accende il LED specificato (0-7)
** Nota: Usa FIOSET per un'operazione atomica (più veloce di |=)
******************************************************************************/
void LED_On(unsigned int num) {
    if(num < LED_NUM) {
        LPC_GPIO2->FIOSET = led_mask[num]; // Scrivere 1 su FIOSET accende il bit
        led_value = (unsigned char)LPC_GPIO2->FIOPIN; // Aggiorna variabile globale
    }
}

/******************************************************************************
** Function name:       LED_Off
** Descriptions:        Spegne il LED specificato (0-7)
** Nota: Usa FIOCLR per un'operazione atomica (più veloce di &= ~)
******************************************************************************/
void LED_Off(unsigned int num) {
    if(num < LED_NUM) {
        LPC_GPIO2->FIOCLR = led_mask[num]; // Scrivere 1 su FIOCLR spegne il bit
        led_value = (unsigned char)LPC_GPIO2->FIOPIN;
    }
}

/******************************************************************************
** Function name:       LED_Toggle
** Descriptions:        Inverte lo stato del LED (Se ON diventa OFF e viceversa)
** Utile per lampeggi senza dover sapere lo stato precedente.
******************************************************************************/
void LED_Toggle(unsigned int num) {
    if(num < LED_NUM) {
        /* Purtroppo non esiste un registro "FIOTOGGLE", quindi dobbiamo leggere e scrivere */
        if (LPC_GPIO2->FIOPIN & led_mask[num]) {
            LPC_GPIO2->FIOCLR = led_mask[num]; // Era acceso, spegni
        } else {
            LPC_GPIO2->FIOSET = led_mask[num]; // Era spento, accendi
        }
        led_value = (unsigned char)LPC_GPIO2->FIOPIN;
    }
}

/******************************************************************************
** Function name:       LED_Out
** Descriptions:        Visualizza un valore intero (0-255) sui LED.
** Esempio: LED_Out(5) accenderà LED0 e LED2 (Binario 00000101).
******************************************************************************/
void LED_Out(unsigned int value) {
    /* Metodo Ottimizzato: Scrittura diretta sulla porta */
    /* 1. Legge lo stato attuale della porta e AZZERA solo i primi 8 bit */
    uint32_t current_state = LPC_GPIO2->FIOPIN & 0xFFFFFF00;
    
    /* 2. Prende solo gli 8 bit meno significativi del valore passato */
    uint32_t new_bits = value & 0x000000FF;
    
    /* 3. Scrive tutto insieme (Atomicita' simulata sulla porta) */
    LPC_GPIO2->FIOPIN = current_state | new_bits;
    
    led_value = (unsigned char)value;
}

/******************************************************************************
** Function name:       LED_Out_rev
** Descriptions:        Visualizza il valore al contrario.
** Il Bit 0 del valore accende il LED 7, il Bit 1 il LED 6, ecc.
******************************************************************************/
void LED_Out_rev(unsigned int value) {
    int i;
    /* Spegniamo tutto prima di iniziare per pulizia */
    LPC_GPIO2->FIOCLR = 0x000000FF; 

    for (i = 0; i < LED_NUM; i++) {
        /* Controllo se l'i-esimo bit di 'value' è 1 */
        if (value & (1 << i)) {
            /* Accendo il LED opposto: (LED_NUM - 1) - i 
             * Es: se i=0 (bit 0), accendo 7-0 = LED 7 */
            LED_On((LED_NUM - 1) - i);
        }
        /* Non serve l'else LED_Off perché abbiamo pulito tutto all'inizio */
    }
}

/******************************************************************************
** Function name:       LED_OnAll
** Descriptions:        Accende tutti i LED
******************************************************************************/
void LED_OnAll(void) {
		int i;
    for(i=0; i<LED_NUM; i++)
        LED_On(i);
}

/******************************************************************************
** Function name:       LED_OffAll
** Descriptions:        Spegne tutti i LED
******************************************************************************/
void LED_OffAll(void) {
    int i;
		for(i=0; i<LED_NUM; i++)
        LED_Off(i);
}

/******************************************************************************
** Function name:       LED_Out_Range
** Descriptions:        Visualizza un valore su un intervallo di LED
** Parameters:          value: valori da visualizzare
**                      from_led_num: primo LED da accendere
**                      to_led_num: ultimo LED da accendere
******************************************************************************/
void LED_Out_Range(unsigned int value, uint8_t from_led_num, uint8_t to_led_num) {
    if(to_led_num < from_led_num || to_led_num >= LED_NUM) return;

		int i, j;
    for(i=from_led_num, j=0; i <= to_led_num; i++, j++) {
        if(value & (1<<j)) LED_On(i);
        else LED_Off(i);
    }
    led_value = (unsigned char)LPC_GPIO2->FIOPIN;
}

/*********************************************************************************************************
** -------------------------------------------------------------------------------------------------------
** SNIPPETS UTILI PER I LED
** -------------------------------------------------------------------------------------------------------
** Copia e incolla questi pezzi di codice nel tuo RIT_IRQHandler (IRQ_RIT.c) o nel Main
** per implementare velocemente funzionalità di debug o effetti visivi.
*********************************************************************************************************/

/* -------------------------------------------------------------------------------------------------------
** 1. EFFETTO SUPERCAR (KITT)
** Descrizione: Un LED scorre avanti e indietro.
** Dove usarlo: Dentro RIT_IRQHandler (IRQ_RIT.c).
** Quando usarlo: Effetto "Idle" (sistema in attesa) o scenografia.
** -------------------------------------------------------------------------------------------------------
*/
/*
	static int pos = 0;
	static int dir = 1;
	
	// Esegui solo ogni X cicli del RIT per rallentare l'effetto (es. ogni 100ms se RIT=50ms)
	static int div = 0;
	if(++div >= 2) { 
		div = 0;
		LED_Out(0); 		// Pulisci
		LED_On(pos);		// Accendi corrente
		
		pos += dir;
		if(pos >= 7) dir = -1; // Rimbalza a destra
		if(pos <= 0) dir = 1;  // Rimbalza a sinistra
	}
*/

/* -------------------------------------------------------------------------------------------------------
** 2. HEARTBEAT (Lampeggio di stato)
** Descrizione: Fa lampeggiare il LED 7 per dire "Sono vivo".
** Dove usarlo: Dentro RIT_IRQHandler.
** Quando usarlo: INDISPENSABILE per sapere se il processore si è bloccato o se il RIT sta girando.
** -------------------------------------------------------------------------------------------------------
*/
/*
	static int heartbeat_tick = 0;
	if(++heartbeat_tick >= 10) { // Ogni 10 cicli RIT (es. 500ms)
		heartbeat_tick = 0;
		LED_Toggle(7); // Inverte stato LED 7
	}
*/

/* -------------------------------------------------------------------------------------------------------
** 3. VISUALIZZATORE DI VARIABILE (Binary Debug)
** Descrizione: Mostra il valore di una variabile sui LED in binario.
** Dove usarlo: Ovunque (Main, Interrupt bottoni, RIT).
** Quando usarlo: Per capire che valore ha una variabile (es. contatore, risultato ADC) senza usare il display.
** -------------------------------------------------------------------------------------------------------
*/
/*
	// Esempio: Visualizza quanti secondi sono passati
	// extern volatile int secondi;
	LED_Out(secondi); 
*/

/* -------------------------------------------------------------------------------------------------------
** 4. FATAL ERROR (Lampeggio Totale)
** Descrizione: Blocca tutto e fa lampeggiare tutti i LED.
** Dove usarlo: In caso di errore critico (es. divisione per zero, timeout hardware).
** -------------------------------------------------------------------------------------------------------
*/
/*
	void Fatal_Error(void) {
		__disable_irq(); // Disabilita tutti gli interrupt
		while(1) {
			LED_Out(0xFF); // Tutti accesi
			for(int i=0; i<1000000; i++); // Delay brutale
			LED_Out(0x00); // Tutti spenti
			for(int i=0; i<1000000; i++); // Delay brutale
		}
	}
*/

/* -------------------------------------------------------------------------------------------------------
** 5. PROGRESS BAR (Barra di caricamento)
** Descrizione: Accende i LED progressivamente da 0 a 7 in base a un valore (0-100%).
** Dove usarlo: Per mostrare il livello di un potenziometro o un caricamento.
** -------------------------------------------------------------------------------------------------------
*/
/*
	// Supponiamo 'val' da 0 a 100
	void Show_Progress(int val) {
		int n_leds = (val * 8) / 100; // Mappa 0-100 su 0-8
		unsigned int mask = 0;
		for(int i=0; i < n_leds; i++) {
			mask |= (1 << i);
		}
		LED_Out(mask);
	}
*/