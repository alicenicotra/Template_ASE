/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_timer.c
** Descriptions:        Gestori delle interruzioni (ISR) per Timer 0, 1, 2, 3
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "LPC17xx.h"
#include "timer.h"

/* INCLUSIONI OPZIONALI
 * Decommenta queste righe se devi interagire con altre periferiche dentro l'interrupt.
 */
//#include "../led/led.h"      // Se hai funzioni dedicate ai LED

/* VARIABILI GLOBALI (VOLATILE)
 * Se una variabile viene modificata dentro un interrupt e letta nel main (o viceversa),
 * DEVE essere dichiarata 'volatile'. Questo dice al compilatore di non ottimizzarla,
 * rileggendola dalla memoria ogni volta.
 */
// extern volatile uint32_t ms_counter; 
// extern volatile uint8_t  flag_event;

/******************************************************************************
** Function name:       TIMER0_IRQHandler
** Descriptions:        Gestore Interrupt Timer 0
** NOTA:                LPC_TIM0->IR è il registro degli Interrupt Flag.
** Scrivendo '1' su un bit, si pulisce (azzera) quel flag.
******************************************************************************/
void TIMER0_IRQHandler (void)
{
    /* Controllo se l'interruzione è stata causata dal Match Register 0 */
    if(LPC_TIM0->IR & 1) 
    { 
        /* --- INIZIO CODICE UTENTE MR0 --- */
        /**************************************************************************
        ** ESEMPIO A: TOGGLE DI UN LED (Heartbeat)
        ** Utile per verificare visivamente che il timer stia girando alla frequenza giusta.
        ** PREREQUISITI: Nel main() devi aver impostato la direzione: LPC_GPIO2->FIODIR |= (1<<0);
        **************************************************************************/
        /*
        LPC_GPIO2->FIOPIN ^= (1 << 0);  // Esegue XOR sul bit 0 del Port 2 (inverte lo stato)
        */

        /**************************************************************************
        ** ESEMPIO B: CONTATORE GLOBALE (es. secondi o millisecondi)
        ** Utile per misurare il tempo o creare delay non bloccanti nel main.
        **************************************************************************/
        /*
        static int ticks = 0; // 'static' mantiene il valore tra le chiamate
        ticks++;
        if(ticks >= 1000) {   // Se configurato a 1ms, questo accade ogni secondo
            ticks = 0;
            // Fai qualcosa ogni secondo (es. aggiorna orologio su GLCD)
            // flag_event = 1; // Segnala al main che è passato un secondo
        }
        */

        /**************************************************************************
        ** ESEMPIO C: TIMER "ONE-SHOT" (Colpo singolo)
        ** Il timer esegue l'operazione una volta e poi si auto-disabilita.
        **************************************************************************/
        /*
        disable_timer(0);  // Ferma il timer immediatamente
        reset_timer(0);    // (Opzionale) Resetta il contatore a 0 per il prossimo utilizzo
        // Esegui l'azione singola qui...
        */
			
        /* --- FINE CODICE UTENTE MR0 --- */

        LPC_TIM0->IR = 1;           // Pulisce il flag di interrupt MR0
    }
    /* Controllo Match Register 1 */
    else if(LPC_TIM0->IR & 2)
    { 
        /* --- INIZIO CODICE UTENTE MR1 --- */
			
        /* --- FINE CODICE UTENTE MR1 --- */
        LPC_TIM0->IR = 2;           // Pulisce il flag MR1
    }
    /* Controllo Match Register 2 */
    else if(LPC_TIM0->IR & 4)
    { 
        /* --- INIZIO CODICE UTENTE MR2 --- */
			
        /* --- FINE CODICE UTENTE MR2 --- */
        LPC_TIM0->IR = 4;           // Pulisce il flag MR2
    }
    /* Controllo Match Register 3 */
    else if(LPC_TIM0->IR & 8)
    { 
        /* --- INIZIO CODICE UTENTE MR3 --- */
			
        /* --- FINE CODICE UTENTE MR3 --- */
        LPC_TIM0->IR = 8;           // Pulisce il flag MR3
    }
    return;
}

/******************************************************************************
** Function name:       TIMER1_IRQHandler
******************************************************************************/
void TIMER1_IRQHandler (void)
{
    if(LPC_TIM1->IR & 1) // MR0
    {
        // Inserisci qui il codice per Timer 1 MR0
        LPC_TIM1->IR = 1; 
    }
    else if(LPC_TIM1->IR & 2) // MR1
    { 
        LPC_TIM1->IR = 2; 
    }
    else if(LPC_TIM1->IR & 4) // MR2
    { 
        LPC_TIM1->IR = 4; 
    }
    else if(LPC_TIM1->IR & 8) // MR3
    { 
        LPC_TIM1->IR = 8; 
    }
    return;
}

/******************************************************************************
** Function name:       TIMER2_IRQHandler
******************************************************************************/
void TIMER2_IRQHandler (void)
{
    if(LPC_TIM2->IR & 1) // MR0
    { 
				/**************************************************************************
        ** ESEMPIO: POLLING PULSANTI (Debounce Software)
        ** Invece di usare gli interrupt EINT dei bottoni (che rimbalzano),
        ** si controlla lo stato dei pin ogni 20-50ms.
        **************************************************************************/
        /*
        // Esempio fittizio lettura bottone INT0 (P2.10)
        if ( (LPC_GPIO2->FIOPIN & (1<<10)) == 0 ) { 
             // Bottone premuto
        }
        */
			
        // Inserisci qui il codice per Timer 2 MR0
        LPC_TIM2->IR = 1; 
    }
    else if(LPC_TIM2->IR & 2) // MR1
    { 
        LPC_TIM2->IR = 2; 
    }
    else if(LPC_TIM2->IR & 4) // MR2
    { 
        LPC_TIM2->IR = 4; 
    }
    else if(LPC_TIM2->IR & 8) // MR3
    { 
        LPC_TIM2->IR = 8; 
    }
    return;
}

/******************************************************************************
** Function name:       TIMER3_IRQHandler
******************************************************************************/
void TIMER3_IRQHandler (void)
{
    if(LPC_TIM3->IR & 1) // MR0
    { 
        // Inserisci qui il codice per Timer 3 MR0
        LPC_TIM3->IR = 1; 
    }
    else if(LPC_TIM3->IR & 2) // MR1
    { 
        LPC_TIM3->IR = 2; 
    }
    else if(LPC_TIM3->IR & 4) // MR2
    { 
        LPC_TIM3->IR = 4; 
    }
    else if(LPC_TIM3->IR & 8) // MR3
    { 
        LPC_TIM3->IR = 8; 
    }
    return;
}