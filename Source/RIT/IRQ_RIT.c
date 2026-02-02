/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_RIT.c
** Descriptions:        Gestione Interrupt RIT con funzionalità avanzate commentate.
** Include gestione pulsanti (debouncing), joystick e utility.
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "LPC17xx.h"
#include "RIT.h"
#include <stdbool.h>
#include <stdio.h> 

/* INCLUDES */
// #include "../led/led.h"     // Per accendere i LED
// #include "../timer/timer.h" // Se devi interagire con i timer hardware

/* VARIABILI GLOBALI VOLATILI
 * 'volatile' serve perché queste variabili sono modificate negli handler EINT (pulsanti)
 * e lette/modificate qui nel RIT. Senza volatile il compilatore potrebbe sbagliare l'ottimizzazione.
 */
volatile int down_0 = 0; // Stato pulsante INT0
volatile int down_1 = 0; // Stato pulsante KEY1
volatile int down_2 = 0; // Stato pulsante KEY2

/* VARIABILI PER TIMER SOFTWARE */
volatile int soft_timer_delay = 0; 

/******************************************************************************
** Function name:       RIT_IRQHandler
** Descriptions:        ISR eseguita periodicamente (es. ogni 50ms)
******************************************************************************/
void RIT_IRQHandler (void)
{           
    /**************************************************************************
    ** UTILITY 1: ADC START CONVERSION (Campionamento Periodico)
    ** COSA FA: Avvia una lettura del potenziometro o microfono a ogni ciclo RIT.
    ** QUANDO USARLA: Se devi monitorare costantemente un sensore (es. volume audio).
    ** NOTA: La lettura vera e propria avverrà nell'interrupt dell'ADC (ADC_IRQHandler).
    **************************************************************************/
    /*
    ADC_start_conversion(); 
    */

    /**************************************************************************
    ** UTILITY 2: LED HEARTBEAT (Lampeggio di stato)
    ** COSA FA: Inverte lo stato di un LED ogni X cicli del RIT.
    ** QUANDO USARLA: Per vedere a occhio nudo se il processore è vivo e il RIT sta girando.
    ** SE RIT = 50ms -> count > 10 significa 500ms (mezzo secondo).
    **************************************************************************/
    /*
    static int led_count = 0;
    led_count++;
    if(led_count > 10) { // Ogni 500ms circa
        led_count = 0;
        // Inverte il bit del LED (es. LED 4 su P2.3)
        // Assicurati che LPC_GPIO2->FIODIR abbia il bit 3 come uscita!
        if(LPC_GPIO2->FIOPIN & (1<<3)) 
             LPC_GPIO2->FIOCLR = (1<<3);
        else 
             LPC_GPIO2->FIOSET = (1<<3);
    }
    */

    /**************************************************************************
    ** UTILITY 3: SOFTWARE TIMER (Ritardo non bloccante)
    ** COSA FA: Decrementa una variabile globale.
    ** QUANDO USARLA: Nel main vuoi aspettare 2 secondi MA vuoi che i pulsanti continuino 
    ** a funzionare? Imposti soft_timer_delay = 40 (se RIT=50ms) nel main e aspetti che diventi 0.
    **************************************************************************/
    /*
    if(soft_timer_delay > 0){
        soft_timer_delay--;
    }
    */
		/*
		// Nel main.c o dove ti serve
		extern volatile int soft_timer_delay; // Dichiari che esiste in IRQ_RIT.c
		void pausa_non_bloccante(void) {
			soft_timer_delay = 40; // Imposta timer (40 * 50ms = 2000ms = 2 sec)
			while(soft_timer_delay > 0) {
					// Qui il processore aspetta, MA il RIT continua a girare
					// Quindi se premi un tasto, viene rilevato (se gestito)
					// Se usi __WFI(); risparmi anche energia
			}
		}
		*/

    /**************************************************************************
    ** GESTIONE PULSANTE INT0 (BUTTON 0)
    **************************************************************************/
    if(down_0 != 0){ 
        down_0++; 
        if((LPC_GPIO2->FIOPIN & (1<<10)) == 0){ // Se premuto (LOW)
            switch(down_0){
                case 2: // PRESSIONE CONFERMATA (Click normale)
                    /* --- CODICE UTENTE QUI --- */
                    
                    /* UTILITY 4: ATTIVAZIONE TIMER
                     * Attiva il Timer0 */
                    // enable_timer(0); 
                    break;

                /* UTILITY 5: PRESSIONE PROLUNGATA (Long Press)
                 * COSA FA: Esegue un'azione diversa se tieni premuto per 1 secondo.
                 * CALCOLO: 1000ms / 50ms (RIT) = 20 cicli.
                 */
                /*
                case 20: 
                    // Esegui azione "Long Press" 
                    break;
                */
                default:
                    break;
            }
        }
        else {  /* RILASCIO PULSANTE */
            down_0 = 0;         
            NVIC_EnableIRQ(EINT0_IRQn);             
            LPC_PINCON->PINSEL4 |= (1 << 20); 
        }
    } 

    /**************************************************************************
    ** GESTIONE PULSANTE KEY1 (BUTTON 1)
    **************************************************************************/
    if(down_1 != 0){
        down_1++;
        if((LPC_GPIO2->FIOPIN & (1<<11)) == 0){ 
            switch(down_1){
                case 2:
                    /* --- CODICE UTENTE KEY1 --- */
                    break;
                default:
                    break;
            }
        }
        else {  /* Rilascio */
            down_1 = 0;         
            NVIC_EnableIRQ(EINT1_IRQn);
            LPC_PINCON->PINSEL4 |= (1 << 22); 
        }
    } 

    /**************************************************************************
    ** GESTIONE PULSANTE KEY2 (BUTTON 2)
    **************************************************************************/
    if(down_2 != 0){
        down_2++;
        if((LPC_GPIO2->FIOPIN & (1<<12)) == 0){ 
            switch(down_2){
                case 2:
                    /* --- CODICE UTENTE KEY2 --- */
                    break;
                default:
                    break;
            }
        }
        else {  /* Rilascio */
            down_2 = 0;     
            NVIC_EnableIRQ(EINT2_IRQn);                         
            LPC_PINCON->PINSEL4 |= (1 << 24); 
        }
    } 

    /**************************************************************************
    ** GESTIONE JOYSTICK (Polling)
    ** Nota: Usiamo variabili statiche perché a differenza dei pulsanti INT0/KEY1/KEY2,
    ** il Joystick non scatena un interrupt esterno iniziale, ma viene controllato solo qui.
    **************************************************************************/
    static int J_up = 0;
    static int J_down = 0;
    static int J_left = 0;
    static int J_right = 0;
    static int J_select = 0; // Click centrale del joystick

    /* --- JOYSTICK UP (P1.29) --- */
    if((LPC_GPIO1->FIOPIN & (1<<29)) == 0){  
        J_up++;
        switch(J_up){
            case 1:
                /* --- CODICE UP --- */
                break;
            /* UTILITY 6: AUTO-REPEAT (Movimento continuo)
             * COSA FA: Se tieni premuto UP, l'azione si ripete ogni tot tempo.
             * QUANDO USARLA: Per scorrere menu o muovere un cursore velocemente.
             */
            /*
            default:
                // Ogni 4 cicli RIT (200ms) ripete l'azione
                if(J_up % 4 == 0) {
                     // Esegui azione movimento
                }
                break;
            */
            default: break;
        }
    }
    else{ J_up = 0; }

    /* --- JOYSTICK DOWN (P1.26) --- */
    if((LPC_GPIO1->FIOPIN & (1<<26)) == 0){  
        J_down++;
        switch(J_down){
            case 1:
                /* --- CODICE DOWN --- */
                break;
            default: break;
        }
    }
    else{ J_down = 0; }

    /* --- JOYSTICK LEFT (P1.27) --- */
    if((LPC_GPIO1->FIOPIN & (1<<27)) == 0){  
        J_left++;
        switch(J_left){
            case 1:
                /* --- CODICE LEFT --- */
                break;
            default: break;
        }
    }
    else{ J_left = 0; }

    /* --- JOYSTICK RIGHT (P1.28) --- */
    if((LPC_GPIO1->FIOPIN & (1<<28)) == 0){  
        J_right++;
        switch(J_right){
            case 1:
                /* --- CODICE RIGHT --- */
                break;
            default: break;
        }
    }
    else{ J_right = 0; }
    
    /* --- JOYSTICK SELECT (P1.25) - Click Centrale --- */
    if((LPC_GPIO1->FIOPIN & (1<<25)) == 0){   
        J_select++;
        switch(J_select){
            case 1:
                /* --- CODICE SELECT --- */
                // Ottimo per mettere in pausa o selezionare menu
                break;
            default: break;
        }
    }
    else{ J_select = 0; }

    /* CHIUSURA OBBLIGATORIA DEL RIT */
    reset_RIT();                // Azzera il contatore per il prossimo ciclo
    LPC_RIT->RICTRL |= 0x1;     // Pulisce il flag di interrupt
    
    return;
}

/*********************************************************************************************************
					VERSIONE AVANZATA 
**--------------------------------------------------------------------------------------------------------
** NOTE:
** Questa versione gestisce:
**  - Pulsanti INT0, KEY1, KEY2 con debounce software
**  - Short press / Long press
**  - Joystick singolo e diagonali (UP, DOWN, LEFT, RIGHT, UP-LEFT, UP-RIGHT, DOWN-LEFT, DOWN-RIGHT)
**  - Auto-repeat opzionale per joystick
**  - Tutto gestito nel RIT, senza bloccare il main loop
**
** COME USARLA:
**  1. Impostare il RIT con periodo fisso (es. 50ms)
**  2. Configurare `long_press_count_1` in base al tempo desiderato
**  3. Scrivere il codice utente nei case corrispondenti
**  4. L’ISR si occupa di debounce e riabilitazione interrupt
*********************************************************************************************************/
#if 0
#include "LPC17xx.h"
#include "RIT.h"
#include "../led/led.h"
//#include "../timer/timer.h"   // Se vuoi interagire con Timer/Delay
//#include "../sample/sample.h" // Se vuoi fare ADC o altre letture

/* ==================== VARIABILI GLOBALI VOLATILI ==================== */
volatile int down_0 = 0;          // Flag pulsante INT0
volatile int down_1 = 0;          // Flag pulsante KEY1
volatile int down_2 = 0;          // Flag pulsante KEY2
volatile int toRelease_down_0 = 0; 
volatile int toRelease_down_1 = 0;
volatile int toRelease_down_2 = 0;

volatile int J_up = 0;            // Joystick UP
volatile int J_down = 0;          // Joystick DOWN
volatile int J_left = 0;          // Joystick LEFT
volatile int J_right = 0;         // Joystick RIGHT
volatile int J_click = 0;         // Joystick SELECT
volatile int J_up_left = 0;       // Joystick diagonali
volatile int J_up_right = 0;
volatile int J_down_left = 0;
volatile int J_down_right = 0;

const int long_press_count_1 = 20; // Numero cicli RIT per long press (~1s se RIT=50ms)

/* ==================== RIT HANDLER ==================== */
void RIT_IRQHandler(void) 
{
    /* Flag per evitare conflitti diagonali/singoli */
    unsigned char UP_LEFT_activated = 0;
    unsigned char UP_RIGHT_activated = 0;
    unsigned char DOWN_LEFT_activated = 0;
    unsigned char DOWN_RIGHT_activated = 0;

    /****************** PULSANTE INT0 ******************/
    if(down_0 != 0) {
        down_0++;
        if((LPC_GPIO2->FIOPIN & (1<<10)) == 0) { // Pulsante premuto (LOW)
            switch(down_0){
                case 2: 
                    // Short press confermata
                    // --- CODICE UTENTE QUI ---
                    toRelease_down_0 = 1;
                    break;
                case long_press_count_1:
                    // Long press
                    // --- CODICE UTENTE LONG PRESS ---
                    break;
                default: break;
            }
        } else { // Pulsante rilasciato
            if(toRelease_down_0){
                // --- CODICE UTENTE RILASCIO PULSANTE ---
                toRelease_down_0 = 0;
            }
            down_0 = 0;
            NVIC_EnableIRQ(EINT0_IRQn);          // Riabilita interrupt
            LPC_PINCON->PINSEL4 |= (1 << 20);    // Modalità EINT0
        }
    }

    /****************** PULSANTE KEY1 ******************/
    if(down_1 != 0) {
        down_1++;
        if((LPC_GPIO2->FIOPIN & (1<<11)) == 0) {
            switch(down_1){
                case 2:
                    // Short press
                    // --- CODICE UTENTE ---
                    toRelease_down_1 = 1;
                    break;
                case long_press_count_1:
                    // Long press
                    // --- CODICE UTENTE LONG PRESS ---
                    break;
                default: break;
            }
        } else {
            if(toRelease_down_1){
                // --- CODICE UTENTE RILASCIO KEY1 ---
                toRelease_down_1 = 0;
            }
            down_1 = 0;
            NVIC_EnableIRQ(EINT1_IRQn);
            LPC_PINCON->PINSEL4 |= (1 << 22);
        }
    }

    /****************** PULSANTE KEY2 ******************/
    if(down_2 != 0) {
        down_2++;
        if((LPC_GPIO2->FIOPIN & (1<<12)) == 0) {
            switch(down_2){
                case 2:
                    // Short press
                    // --- CODICE UTENTE ---
                    toRelease_down_2 = 1;
                    break;
                case long_press_count_1:
                    // Long press
                    // --- CODICE UTENTE LONG PRESS ---
                    break;
                default: break;
            }
        } else {
            if(toRelease_down_2){
                // --- CODICE UTENTE RILASCIO KEY2 ---
                toRelease_down_2 = 0;
            }
            down_2 = 0;
            NVIC_EnableIRQ(EINT2_IRQn);
            LPC_PINCON->PINSEL4 |= (1 << 24);
        }
    }

    /****************** JOYSTICK DIAGONALI ******************/
    // UP-LEFT
    if(((LPC_GPIO1->FIOPIN & (1<<27)) == 0) && ((LPC_GPIO1->FIOPIN & (1<<29)) == 0)) {
        J_up_left++;
        UP_LEFT_activated = 1;
        switch(J_up_left){
            case 1: 
                // Short press UP-LEFT
                // --- CODICE UTENTE ---
                break;
            case long_press_count_1:
                // Long press UP-LEFT
                // --- CODICE UTENTE LONG PRESS ---
                break;
            default: break;
        }
    } else { J_up_left = 0; }

    // UP-RIGHT
    if(((LPC_GPIO1->FIOPIN & (1<<28)) == 0) && ((LPC_GPIO1->FIOPIN & (1<<29)) == 0)) {
        J_up_right++;
        UP_RIGHT_activated = 1;
        switch(J_up_right){
            case 1:
                // Short press UP-RIGHT
                // --- CODICE UTENTE ---
                break;
            case long_press_count_1:
                // Long press UP-RIGHT
                // --- CODICE UTENTE LONG PRESS ---
                break;
            default: break;
        }
    } else { J_up_right = 0; }

    // DOWN-LEFT
    if(((LPC_GPIO1->FIOPIN & (1<<27)) == 0) && ((LPC_GPIO1->FIOPIN & (1<<26)) == 0)) {
        J_down_left++;
        DOWN_LEFT_activated = 1;
        switch(J_down_left){
            case 1:
                // Short press DOWN-LEFT
                // --- CODICE UTENTE ---
                break;
            case long_press_count_1:
                // Long press DOWN-LEFT
                // --- CODICE UTENTE LONG PRESS ---
                break;
            default: break;
        }
    } else { J_down_left = 0; }

    // DOWN-RIGHT
    if(((LPC_GPIO1->FIOPIN & (1<<26)) == 0) && ((LPC_GPIO1->FIOPIN & (1<<28)) == 0)) {
        J_down_right++;
        DOWN_RIGHT_activated = 1;
        switch(J_down_right){
            case 1:
                // Short press DOWN-RIGHT
                // --- CODICE UTENTE ---
                break;
            case long_press_count_1:
                // Long press DOWN-RIGHT
                // --- CODICE UTENTE LONG PRESS ---
                break;
            default: break;
        }
    } else { J_down_right = 0; }

    /****************** JOYSTICK SINGOLI ******************/
    // UP
    if(UP_LEFT_activated==0 && UP_RIGHT_activated==0){
        if((LPC_GPIO1->FIOPIN & (1<<29)) == 0) {
            J_up++;
            switch(J_up){
                case 1:
                    // Short press UP
                    // --- CODICE UTENTE ---
                    break;
                case long_press_count_1:
                    // Long press UP
                    // --- CODICE UTENTE LONG PRESS ---
                    break;
                default: break;
            }
        } else { J_up = 0; }
    }

    // DOWN
    if(DOWN_LEFT_activated==0 && DOWN_RIGHT_activated==0){
        if((LPC_GPIO1->FIOPIN & (1<<26)) == 0) {
            J_down++;
            switch(J_down){
                case 1:
                    // Short press DOWN
                    // --- CODICE UTENTE ---
                    break;
                case long_press_count_1:
                    // Long press DOWN
                    // --- CODICE UTENTE LONG PRESS ---
                    break;
                default: break;
            }
        } else { J_down = 0; }
    }

    // RIGHT
    if(DOWN_RIGHT_activated==0 && UP_RIGHT_activated==0){
        if((LPC_GPIO1->FIOPIN & (1<<28)) == 0){
            J_right++;
            switch(J_right){
                case 1:
                    // Short press RIGHT
                    // --- CODICE UTENTE ---
                    break;
                case long_press_count_1:
                    // Long press RIGHT
                    // --- CODICE UTENTE LONG PRESS ---
                    break;
                default: break;
            }
        } else { J_right = 0; }
    }

    // LEFT
    if(UP_LEFT_activated==0 && DOWN_LEFT_activated==0){
        if((LPC_GPIO1->FIOPIN & (1<<27)) == 0){
            J_left++;
            switch(J_left){
                case 1:
                    // Short press LEFT
                    // --- CODICE UTENTE ---
                    break;
                case long_press_count_1:
                    // Long press LEFT
                    // --- CODICE UTENTE LONG PRESS ---
                    break;
                default: break;
            }
        } else { J_left = 0; }
    }

    // CLICK
    if((LPC_GPIO1->FIOPIN & (1<<25)) == 0){
        J_click++;
        switch(J_click){
            case 1:
                // Short press SELECT
                // --- CODICE UTENTE ---
                break;
            case long_press_count_1:
                // Long press SELECT
                // --- CODICE UTENTE LONG PRESS ---
                break;
            default: break;
        }
    } else { J_click = 0; }

    /* ==================== CHIUSURA ISR ==================== */
    LPC_RIT->RICTRL |= 0x1;   // Pulisce flag interrupt RIT
    return;
}
#endif

/******************************************************************************
** End Of File
******************************************************************************/