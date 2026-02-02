/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           led.h
** Descriptions:        Prototipi per la gestione dei LED (Porta 2, Pin 0-7)
**--------------------------------------------------------------------------------------------------------      
*********************************************************************************************************/
#ifndef __LED_H
#define __LED_H

#include "LPC17xx.h"

/* Numero di LED disponibili sulla scheda */
#define LED_NUM     8               

/* Variabile globale (utile per debug) che tiene traccia dello stato attuale */
extern unsigned char led_value;

/* --- FUNZIONI DI INIZIALIZZAZIONE (Hardware) --- */
void LED_init(void);
void LED_deinit(void);

/* --- FUNZIONI OPERATIVE (High Level) --- */

/* Accende un singolo LED (0-7) */
void LED_On(unsigned int num);

/* Spegne un singolo LED (0-7) */
void LED_Off(unsigned int num);

/* Inverte lo stato di un LED (Toggle) */
void LED_Toggle(unsigned int num);

/* Visualizza un valore binario (0-255) sui LED */
void LED_Out(unsigned int value);

/* Visualizza un valore binario in ordine inverso (Bit 0 su LED 7, Bit 7 su LED 0) */
void LED_Out_rev(unsigned int value);

/* Accende tutti i LED */
void LED_OnAll(void);

/* Spegne tutti i LED */
void LED_OffAll(void);

/* Visualizza un valore su un intervallo di LED (from_led_num -> to_led_num) */
void LED_Out_Range(unsigned int value, uint8_t from_led_num, uint8_t to_led_num);

#endif /* end __LED_H */
