/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           lib_button.c
** Descriptions:        Inizializzazione dei pulsanti come External Interrupts
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "button.h"
#include "LPC17xx.h"

/**
 * @brief  Funzione che inizializza i bottoni
 * Configura PINSEL4, EXTMODE e NVIC.
 */
void BUTTON_init(void) {

  /* CONFIGURAZIONE PIN FUNCTION (PINSEL4)
   * PINSEL4 gestisce la porta P2.0 a P2.15.
   * Ogni pin ha 2 bit.
   * 00 = GPIO (Default)
   * 01 = EINT (External Interrupt)
   */

  /* --- INT0 (Button 0) su P2.10 --- */
  LPC_PINCON->PINSEL4    |= (1 << 20);     /* Bit 20 a 1 -> Funzione "01" (EINT0) */
  LPC_GPIO2->FIODIR      &= ~(1 << 10);    /* P2.10 Input (prudenziale, anche se EINT lo forza) */

  /* --- KEY1 (Button 1) su P2.11 --- */
  LPC_PINCON->PINSEL4    |= (1 << 22);     /* Bit 22 a 1 -> Funzione "01" (EINT1) */
  LPC_GPIO2->FIODIR      &= ~(1 << 11);    /* P2.11 Input */
  
  /* --- KEY2 (Button 2) su P2.12 --- */
  LPC_PINCON->PINSEL4    |= (1 << 24);     /* Bit 24 a 1 -> Funzione "01" (EINT2) */
  LPC_GPIO2->FIODIR      &= ~(1 << 12);    /* P2.12 Input */

  /* CONFIGURAZIONE MODALITÀ DI SCATTO (EXTMODE)
   * 0 = Level Sensitive (scatta finché tieni premuto)
   * 1 = Edge Sensitive (scatta solo sul fronte di discesa/salita)
   * Bit 0->EINT0, Bit 1->EINT1, Bit 2->EINT2
   * Noi vogliamo Edge Sensitive (scatta appena premiamo).
   */
  LPC_SC->EXTMODE = 0x7; // 111 in binario -> Tutti e 3 Edge Sensitive

  /* CONFIGURAZIONE POLARITÀ (EXTPOLAR) - Opzionale (default è 0)
   * 0 = Falling Edge (scatta quando passa da 3.3V a 0V -> Pressione tasto)
   * 1 = Rising Edge (scatta al rilascio)
   * I tasti sulla LandTiger chiudono a massa, quindi Falling Edge è corretto.
   */
  LPC_SC->EXTPOLAR &= ~(0x7); // Assicuro che siano a 0 (Falling Edge)

  /* ABILITAZIONE INTERRUPT (NVIC) 
   * Impostiamo le priorità. Numeri più bassi = priorità più alta.
   */
  
  // Button 2 (Priority 1 - Alta)
  NVIC_EnableIRQ(EINT2_IRQn);              
  NVIC_SetPriority(EINT2_IRQn, 1);         

  // Button 1 (Priority 2 - Media)
  NVIC_EnableIRQ(EINT1_IRQn);              
  NVIC_SetPriority(EINT1_IRQn, 2);                
  
  // Button 0 (Priority 3 - Bassa) -> Nota: nel tuo codice originale era 0 (Altissima).
  // Di solito i pulsanti hanno priorità più bassa rispetto a Timer e Audio.
  NVIC_EnableIRQ(EINT0_IRQn);              
  NVIC_SetPriority(EINT0_IRQn, 3);                
}