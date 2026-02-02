/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_button.c
** Descriptions:        Interrupt Service Routines per EINT0, EINT1, EINT2
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "button.h"
#include "LPC17xx.h"

/* Variabili esterne definite in IRQ_RIT.c (o main.c) 
 * Queste bandiere dicono al RIT: "Ehi, qualcuno ha premuto il tasto!"
 */
extern volatile int down_0;
extern volatile int down_1;
extern volatile int down_2;

/******************************************************************************
** Function name:       EINT0_IRQHandler
** Descriptions:        Handler per INT0 (Button 0) - P2.10
******************************************************************************/
void EINT0_IRQHandler (void)      
{       
    /* 1. Segnala la pressione al RIT */
    down_0 = 1; 
	
		/**************************************************************************
    ** UTILITY 1: DEBUG HARDWARE IMMEDIATO (Test se il bottone funziona)
    ** COSA FA: Accende un LED appena il processore sente l'impulso elettrico.
    ** QUANDO USARLA: Se il codice nel RIT non parte e non capisci se è colpa 
    ** del software (RIT) o dell'hardware (bottone rotto/scollegato).
    ** Se il LED si accende ma il programma non fa nulla, il problema è nel RIT.
    **************************************************************************/
    /*
    LPC_GPIO2->FIOSET = (1 << 0); // Accendi LED su P2.0 (esempio)
    */

    /* 2. Disabilita questo interrupt nel NVIC 
     * Perché? Per evitare rimbalzi (bouncing). Se non lo spegni,
     * l'interrupt scatterebbe 10 volte in 1ms.
     * Sarà il RIT a riabilitarlo quando il tasto viene rilasciato.
     */
		/* 
		Questo codice disabilita l'interrupt (NVIC_DisableIRQ) e cambia il pin in GPIO (PINSEL4 &= ...).
		Nel file IRQ_RIT.c c'è la logica inversa nel blocco else che permette di creare il sistema di debaucing perfetto
		else { // button released
			down_0=0;           
			NVIC_EnableIRQ(EINT0_IRQn);             // Riabilita interrupt 
			LPC_PINCON->PINSEL4 |= (1 << 20);       // Ritorna in modalità EINT 
		}
		*/
    NVIC_DisableIRQ(EINT0_IRQn);        

    /* 3. Cambia il pin da EINT a GPIO normale 
     * Questo permette al RIT di leggere il livello del pin (0 o 1) 
     * senza scatenare nuovi interrupt.
     * PINSEL4 bit 20 a 0 -> GPIO
     */
    LPC_PINCON->PINSEL4    &= ~(1 << 20);     
    
    /* 4. Pulisci il flag di Interrupt 
     * Bit 0 del registro EXTINT corrisponde a EINT0.
     */
    LPC_SC->EXTINT |= (1 << 0);     
}


/******************************************************************************
** Function name:       EINT1_IRQHandler
** Descriptions:        Handler per KEY1 (Button 1) - P2.11
******************************************************************************/
void EINT1_IRQHandler (void)      
{
    down_1 = 1;
	
		/* UTILITY: FLAG DI START VELOCE
     * Se devi avviare un cronometro, fallo qui per la massima precisione,
     * ma gestisci lo stop/visualizzazione nel RIT.
     */
    // start_timer_flag = 1;
	
    NVIC_DisableIRQ(EINT1_IRQn);        
    LPC_PINCON->PINSEL4    &= ~(1 << 22);     /* GPIO mode P2.11 */
    
    LPC_SC->EXTINT |= (1 << 1);     /* Clear flag EINT1 (Bit 1) */
}

/******************************************************************************
** Function name:       EINT2_IRQHandler
** Descriptions:        Handler per KEY2 (Button 2) - P2.12
******************************************************************************/
void EINT2_IRQHandler (void)      
{
    down_2 = 1;
    NVIC_DisableIRQ(EINT2_IRQn);        
    LPC_PINCON->PINSEL4    &= ~(1 << 24);     /* GPIO mode P2.12 */
    
    LPC_SC->EXTINT |= (1 << 2);     /* Clear flag EINT2 (Bit 2) */    
}


/*============================================================================
  VERSIONE AVANZATA CON DEBOUNCE
  USO: 
    - Evita che l'ISR venga chiamata più volte per un singolo click a causa dei rimbalzi meccanici.
    - Disabilita l'interrupt e lo riabilita dopo un tempo minimo usando un timer (es. RIT)
============================================================================*/
/* Decommentare e usare RIT per riattivare IRQ */
/* 

void EINT0_IRQHandler (void)
{
    down_0 = 1;                                 // Flag al main
    NVIC_DisableIRQ(EINT0_IRQn);                // Disabilita IRQ per debounce
    LPC_SC->EXTINT = (1 << 0);                  // Pulisce flag

    // NOTA: Riabilitare IRQ dopo 20-50ms usando RIT
    // init_RIT(500000);  // se PCLK = 25MHz -> 50ms
    // enable_RIT();
}

void EINT1_IRQHandler (void)
{
    down_1 = 1;
    NVIC_DisableIRQ(EINT1_IRQn);                
    LPC_SC->EXTINT = (1 << 1);                  // Pulisce flag

    // Riabilita IRQ tramite RIT
		// init_RIT(500000); 
    // enable_RIT();
}

void EINT2_IRQHandler (void)
{
    down_2 = 1;
    NVIC_DisableIRQ(EINT2_IRQn);                
    LPC_SC->EXTINT = (1 << 2);                  // Pulisce flag

    // Riabilita IRQ tramite RIT
		// init_RIT(500000); 
    // enable_RIT();
}

*/

/*============================================================================
  COME USARE LA VERSIONE DEBOUNCE:
  1. Abilitare la sezione #if 0 ? #if 1 o decommentare.
  2. Configurare un RIT o Timer per un tempo di “debounce” (~20-50ms).
  3. Allo scadere del tempo, nel RIT Handler:
        - Riabilitare l’IRQ corrispondente:
            NVIC_EnableIRQ(EINT0_IRQn);
            NVIC_EnableIRQ(EINT1_IRQn);
            NVIC_EnableIRQ(EINT2_IRQn);
  4. In questo modo, anche se il pulsante rimbalza, la ISR viene chiamata **una sola volta**.
============================================================================*/