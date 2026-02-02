/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           sample.c
** Descriptions:        Main application template per LPC17xx (LandTiger)
** Include gestione Init, Loop Infinito e Snippets utili.
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "LPC17xx.h"
#include <stdio.h>
#include "GLCD/GLCD.h" 
#include "TouchPanel/TouchPanel.h"
#include "timer/timer.h"
#include "RIT/RIT.h"
#include "led/led.h"
#include "button/button.h"
#include "joystick/joystick.h"
//#include "adc/adc.h"

#ifdef SIMULATOR
extern uint8_t ScaleFlag; 
#endif

/* -------------------------------------------------------------------------
   PROTOTIPI FUNZIONI ASSEMBLY 
   Devono essere dichiarate 'extern' per essere chiamate dal C.
   ------------------------------------------------------------------------- */
// extern void my_asm_function(int a, int b); // Esempio per altre funzioni

/* Variabili Globali */
//volatile int flag_main = 0; 

int main(void)
{
    /* 1. SYSTEM INIT: Configura il Clock (PLL) a 100MHz (di solito) */
    SystemInit(); 

    /* =========================================================================
       SEZIONE 1: POWER CONTROL FOR PERIPHERALS (PCONP)
       IMPORTANTE: Di default molte periferiche sono SPENTE per risparmiare energia.
       Se il timer o l'ADC non partono, controlla qui!
       ========================================================================= */
    // LPC_SC->PCONP |= (1 << 12); // Enable ADC power
    // LPC_SC->PCONP |= (1 << 22); // Enable TIMER 2
    // LPC_SC->PCONP |= (1 << 23); // Enable TIMER 3   
    
    /* =========================================================================
       SEZIONE 2: INIZIALIZZAZIONE PERIFERICHE
       ========================================================================= */
    /* --- LED --- */
    // LED_init(); 
    
    /* --- BUTTONS --- */
    // BUTTON_init(); 
    
    /* --- JOYSTICK --- */
    // joystick_init(); 

    /* --- RIT --- */
    /* 50ms = 5.000.000 ticks @ 100MHz */
    // init_RIT(0x004C4B40); 
    // enable_RIT();

    /* --- LCD & TOUCH --- */
    /* Nota: TouchPanel_Calibrate blocca il codice finché non tocchi lo schermo! */
    // LCD_Initialization();
    // LCD_Clear(White);
    // TP_Init(); 
    // TouchPanel_Calibrate(); 
    
    /* --- TIMER --- */
    /* FORMULA TIMER MATCH REGISTER:
       MR = T_desiderato_sec * Frequenza_Timer
       Di solito F_Timer = 25MHz (CCLK/4).
       
       Esempio 1 Secondo: 1 * 25.000.000 = 25.000.000 (0x017D7840)
       Esempio 0.5 Sec:   0.5 * 25.000.000 = 12.500.000 (0x00BEBC20)
       
       Config init_timer: (timer_num, prescaler, match_reg, SRI, value)
       SRI (Stop-Reset-Interrupt):
       1 = Interrupt (continua a contare)
       2 = Reset (riparte da 0 ma non chiama interrupt)
       3 = Interrupt + Reset (Classico timer periodico)
    */
		/* ================= TIMER SRI CHEAT SHEET =================

				SRI = 0 (000) -> NIENTE
				- Il timer continua a contare
				- Nessun interrupt
				- Nessun reset

				SRI = 1 (001) -> INTERRUPT ONLY
				- Genera interrupt
				- NON si resetta
				- Continua a contare
				=> utile per timestamp o contatori

				SRI = 2 (010) -> RESET ONLY
				- Nessun interrupt
				- Si resetta a 0
				- Continua a contare
				=> timer “silenzioso”

				SRI = 3 (011) -> INTERRUPT + RESET (STANDARD)
				- Interrupt periodico
				- Reset automatico
				- Continua a contare
				=> TIMER PERIODICO CLASSICO

				SRI = 4 (100) -> STOP ONLY
				- Nessun interrupt
				- NON resetta

				SRI = 5 (101) -> INTERRUPT + STOP
				- Interrupt una sola volta
				- Si ferma al match

				SRI = 6 (110) -> RESET + STOP
				- Reset e stop
				- Nessun interrupt

				SRI = 7 (111) -> INTERRUPT + RESET + STOP
				- Interrupt una volta
				- Reset
				- Stop

			 */
				/*
				 CONVERSIONI RAPIDE
				 -----------------------------------------------------------------------------------------------------
				 1 s  = 1000 ms
				 1 ms = 1000 µs
				 1 µs = 1000 ns

				 1 MHz = 1.000.000 Hz
				 1 kHz = 1.000 Hz
				*/
    
    // init_timer(0, 0, 0, 3, 25000000); // Timer0, MR0, Reset+Int, 1 sec
    // enable_timer(0);
		
		/* =====================================================================================
			 FORMULE BASE – FREQUENZA, PERIODO, TEMPO 
			 =====================================================================================

			 RELAZIONI FONDAMENTALI:
			 ----------------------
			 Frequenza (Hz):        f = 1 / T
			 Periodo (secondi):    T = 1 / f

			 TEMPO DI UN EVENTO PERIODICO:
			 -----------------------------
			 T_evento = T = 1 / f

			 FREQUENZA DI UN EVENTO:
			 ----------------------
			 f_evento = 1 / T

			 LAMPEGGIO LED (ON/OFF SIMMETRICO):
			 ---------------------------------
			 Periodo totale:       T = T_ON + T_OFF

			 Se ON = OFF:
			 T_ON  = T / 2
			 T_OFF = T / 2

			 DA FREQUENZA A TEMPO ON/OFF:
			 ----------------------------
			 Dato f:
			 T = 1 / f
			 T_ON  = 1 / (2·f)
			 T_OFF = 1 / (2·f)

			 DA PERIODO A FREQUENZA:
			 ----------------------
			 Dato T:
			 f = 1 / T

			 =====================================================================================
			 ESEMPI TIPICI DA ESAME:
			 =====================================================================================

			 LED a 1 Hz:
			 f = 1 Hz
			 T = 1 s
			 T_ON = 0.5 s
			 T_OFF = 0.5 s

			 LED a 2 Hz:
			 T = 0.5 s
			 T_ON = 0.25 s
			 T_OFF = 0.25 s

			 LED a 10 Hz:
			 T = 0.1 s = 100 ms
			 T_ON = 50 ms
			 T_OFF = 50 ms

			 LED con periodo 200 ms:
			 T = 0.2 s
			 f = 5 Hz
			 T_ON = 100 ms
			 T_OFF = 100 ms
		*/


    /* =========================================================================
       SEZIONE 3: SNIPPETS LOGICI UTILI (Da copiare/usare all'occorrenza)
       ========================================================================= */
    /* --- BITWISE OPERATIONS CHEAT SHEET ---
       VAR & (1<<n)   -> LEGGE il bit n (ritorna 0 o !=0)
       VAR |= (1<<n)  -> SETTA il bit n a 1
       VAR &= ~(1<<n) -> PULISCE il bit n a 0
       VAR ^= (1<<n)  -> INVERTE (Toggle) il bit n
       ~VAR           -> NOT bit a bit (Complemento a 1)
    */

    /* --- CHIAMATA ASSEMBLY --- 
       Codice C che usa la funzione esterna definita in assembly
    */
    /*
    unsigned char state = 0xAA; // Stato iniziale seed
    unsigned char taps = 0x1D;  // Configurazione taps (es. 29)
    int output_bit = 0;
    
    state = next_state(state, taps, &output_bit);
    
    // Visualizza su LED
    LED_Out(state);
    */

    /* --- ALGORITMO BUBBLE SORT --- 
       Da usare se richiesto di ordinare vettori.
    */
    /*
    #define N 5
    int lista[N] = {6, 2, 4, 7, 1};
    int i, j, temp;

    for(i = 0; i < N - 1; i++){
        for(j = 0; j < N - i - 1; j++){
            if(lista[j] > lista[j + 1]){ // Usa < per decrescente
                temp = lista[j];
                lista[j] = lista[j + 1];
                lista[j + 1] = temp;
            }
        }
    }
    */
		
		/* =========================================================================
       SNIPPET AREA: ESEMPI PRONTI ALL'USO 
       ========================================================================= */
    /* --- GESTIONE STATI (Macchina a Stati Finiti) --- 
       Fondamentale per i giochi (Menu -> Gioco -> Game Over)
    */
    /*
    enum { STATE_MENU, STATE_PLAY, STATE_GAMEOVER };
    int current_state = STATE_MENU;

    // Nel while(1):
    switch(current_state) {
        case STATE_MENU:
            if(down_0) { // Se premo bottone 0
                 LCD_Clear(White);
                 current_state = STATE_PLAY;
                 down_0 = 0; // Reset flag
            }
            break;
        case STATE_PLAY:
            // Logica gioco...
            break;
    }
    */

    while (1)   
    {
        /* Wait For Interrupt: Mette la CPU in pausa finché non arriva 
           un interrupt (Timer, RIT, Button). Risparmia batteria e riduce calore. 
           Toglilo se devi fare polling continuo nel while(1).
        */
        __ASM("wfi");
    }
}

/* =========================================================================
   IMPLEMENTAZIONE HELPER FUNCTIONS
   ========================================================================= */


/*********************************************************************************************************
      END FILE
*********************************************************************************************************/