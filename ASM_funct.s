; ****************************************************************************
; * Name: ASM_funct.s
; * Description: Raccolta completa di funzioni Assembly per esame LPC17xx
; * Platform: Cortex-M3 (LPC1768)
; ****************************************************************************

; --- DIRETTIVE STANDARD ---
        PRESERVE8                   ; Allineamento stack a 8 byte
        THUMB                       ; Set di istruzioni Thumb (Obbligatorio per Cortex-M)

; --- IMPORTAZIONI C ---
        IMPORT  __aeabi_fdiv        ; Importa funzione divisione float (se serve)

; --- DEFINIZIONE AREA CODICE ---
        AREA    |.text|, CODE, READONLY
			
; ============================================================================
; UTILIZZO DELLE VARIABILI DEFINITE IN C
; ============================================================================
; 1. Dichiarazione in C:
;      // Variabile singola
;      extern uint32_t my_counter;
;      // Array di byte o word
;      extern uint8_t buffer[10];
; 
; 2. Dichiarazione in Assembly:
;      IMPORT my_counter
;      IMPORT buffer
;
; 3. Lettura / Scrittura:
;      LDR     r0, =my_counter   ; r0 punta a my_counter
;      LDR     r1, [r0]          ; legge valore della variabile
;      ADD     r1, r1, #1        ; operazione qualsiasi
;      STR     r1, [r0]          ; scrive valore aggiornato
;
;      LDR     r0, =buffer
;      LDRB    r1, [r0, #3]      ; legge buffer[3]
;      MOV     r1, #0xFF
;      STRB    r1, [r0, #3]      ; scrive buffer[3] = 0xFF
;
; 4. Offset per array:
;      - Array di byte: usa direttamente l’indice
;      - Array di word (uint32_t): moltiplica l’indice per 4
;        esempio: LDR r2, [r0, r3, LSL #2] ; r3 = indice
;
; 5. Registri extra:
;      - Se servono più registri di quelli standard (r0-r3), salvare e ripristinare
;        registri non volatili (r4-r11) usando lo stack:
;           PUSH {r4-r7, lr}
;           ... codice ...
;           POP  {r4-r7, pc}
;
; 6. Risultati delle funzioni:
;      - Il valore di ritorno va sempre in r0
;      - Puntatori o array si passano in r0


; ============================================================================
; PASSAGGIO DELLE VARIABILI COME ARGOMENTI DELLA FUNZIONE
; ============================================================================
; 1. Registri utilizzati per gli argomenti:
;      - r0, r1, r2, r3 sono usati per i primi 4 argomenti.
;      - Se ci sono più di 4 argomenti, i successivi vanno nello stack.
;
; 2. Tipi comuni:
;      - int, uint32_t, puntatori -> r0, r1, r2, r3
;      - float/double -> passati in r0-r1 (float singolo) o su stack (double)
;
; 3. Lettura / scrittura degli argomenti:
;      - Il primo argomento della funzione è in r0
;      - Il secondo argomento in r1, il terzo in r2, il quarto in r3
;
; 4. Esempio:
;      // Funzione C: int sum(int a, int b, int c)
;      int sum(int a, int b, int c) { return a+b+c; }
;
;      ; In ASM:
;      sum PROC
;          ; r0 = a, r1 = b, r2 = c
;          ADD r0, r0, r1      ; r0 = a + b
;          ADD r0, r0, r2      ; r0 = a + b + c
;          BX  lr              ; ritorno (r0 contiene risultato)
;      ENDP
;
; 5. Puntatori come argomenti:
;      - Se passi un array: int fun(uint32_t *arr, int n)
;          r0 = puntatore ad arr
;          r1 = n
;      - Per leggere elementi: LDR / STR con offset
;          LDR r2, [r0, r3, LSL #2] ; r3 = indice

; ============================================================================
; PASSAGGIO DI UN VETTORE (ARRAY)
;      // C: int sum_array(int *arr, int n)
;      // Ritorna la somma dei primi n elementi
;
;      ; In ASM:
;      sum_array PROC
;          ; r0 = puntatore all'array arr
;          ; r1 = numero elementi n
;          
;          MOV   r2, #0        ; somma = 0
;          MOV   r3, #0        ; indice i = 0
;
;loop_arr
; Controllo fine array
;          CMP   r3, r1
;          BGE   end_arr
; Leggi elemento arr[i] (32 bit)
;          LDR   r4, [r0, r3, LSL #2]  ; r4 = arr[i]
; Aggiungi alla somma
;          ADD   r2, r2, r4
; Incrementa indice
;          ADD   r3, r3, #1
;          B     loop_arr
;
;end_arr
; Ritorna risultato in r0
;          MOV   r0, r2
;          BX    lr
;      ENDP
;
; Nota:
;  - Puntatore all'array è sempre in r0
;  - Per leggere arr[i] si usa: LDR rX, [r0, r3, LSL #2]
;    LSL #2 perché gli elementi sono word (4 byte)
			

; ----------------------------------------------------------------------------
; Function: compress
; Descrizione: Calcola le differenze tra elementi adiacenti.
;              Conta quante differenze sono negative.
; Input:  R0 = Vettore, R1 = Num Elementi, R2 = Vettore Risultato
; Output: R0 = Conteggio differenze negative
; ----------------------------------------------------------------------------
        EXPORT compress
compress PROC
        MOV     r12, sp             ; (Opzionale) Frame pointer
        STMFD   sp!, {r4-r8,r10-r11,lr}

        MOV     R3, #0              ; Azzera accumulatore (non usato qui ma buona prassi)
        MOV     R4, #0              ; Indice i = 0
        MOV     R7, #0              ; Count (risultato) = 0
        
loop_cp LDRB    R5, [R0, R4]        ; R5 = VETT[i]
        ADD     R9, R4, #1          ; Indice i+1
        LDRB    R6, [R0, R9]        ; R6 = VETT[i+1]
        
        SUBS    R8, R5, R6          ; R8 = VETT[i] - VETT[i+1]
        STRB    R8, [R2, R4]        ; Salva risultato in RES[i]
        
        ; Se risultato < 0 (Flag Negative=1), incrementa count
        ADDMI   R7, R7, #1          
        
        ADD     R4, #1              ; i++
        CMP     R4, R1              ; if i == numElementi (Attenzione: meglio N-1 per le differenze)
        BNE     loop_cp

        MOV     R0, R7              ; Return count in R0
        
        LDMFD   sp!, {r4-r8,r10-r11,pc}
        ENDP

; ----------------------------------------------------------------------------
; Function: sopra_la_media
; Descrizione: Calcola la media e conta quanti valori sono maggiori della media.
; Input:  R0 = Vettore, R1 = Num Elementi
; Output: R0 = Conteggio
; ----------------------------------------------------------------------------
        EXPORT sopra_la_media
sopra_la_media PROC
        MOV     r12, sp
        STMFD   sp!, {r4-r8,r10-r11,lr}

        MOV     R3, #0              ; Somma = 0
        MOV     R4, #0              ; i = 0
        
        ; --- Fase 1: Calcolo Somma ---
loop2   LDRB    R2, [R0, R4]        ; Carica byte
        ADD     R3, R3, R2          ; Somma += val
        ADD     R4, R4, #1          ; i++
        CMP     R4, R1
        BNE     loop2
        
        ; --- Fase 2: Calcolo Media ---
        UDIV    R3, R3, R1          ; R3 = Somma / N (Divisione Intera Hardware)
        
        ; --- Fase 3: Conteggio ---
        MOV     R4, #0              ; Reset i = 0
        MOV     R5, #0              ; Count = 0
        
loop_chk LDRB   R2, [R0, R4]        ; Carica byte
        CMP     R2, R3              ; Confronta val con Media (R3)
        ADDGT   R5, R5, #1          ; Se > Media, count++
        
        ADD     R4, R4, #1          ; i++
        CMP     R4, R1
        BNE     loop_chk

        MOV     R0, R5              ; Return count
        
        LDMFD   sp!, {r4-r8,r10-r11,pc}
        ENDP

; ----------------------------------------------------------------------------
; Function: get_and_sort
; Descrizione: Inserimento ordinato di un valore in un array (tipo Insertion Step).
; Input:  R0 = Vettore, R1 = Valore da inserire, R2 = Indice inserimento/N
; ----------------------------------------------------------------------------
        EXPORT  get_and_sort
get_and_sort PROC
        MOV     r12, sp
        STMFD   sp!, {r4-r8,r10-r11,lr}              
                
        ADD     R2, R0, R2          ; R2 punta alla fine dell'array (Base + Offset)
        
loopSort SUB    R2, R2, #1          ; Indice corrente (j)
        SUB     R3, R2, #1          ; Indice precedente (j-1)
        
        CMP     R0, R3              ; Controllo bounds (siamo arrivati all'inizio?)
        BHI     fine_sort           ; Se R3 < R0 (Base), finito
        
        LDRB    R4, [R3]            ; Carica VETT[j-1]
        CMP     R4, R1              ; Confronta con Valore
        BHI     fine_sort           ; Se VETT[j-1] > Valore, ho trovato il posto
        
        STRB    R4, [R2]            ; Altrimenti Shift: VETT[j] = VETT[j-1]
        B       loopSort
        
fine_sort STRB  R1, [R2]            ; Inserisci valore nella posizione trovata
        MOV     R0, R1              ; Return value (opzionale)

        LDMFD   sp!, {r4-r8,r10-r11,pc}
        ENDP

; ----------------------------------------------------------------------------
; Function: contatore_inrange
; Descrizione: Conta valori compresi tra il Primo e l'Ultimo elemento del vettore.
; Input:  R0 = Vettore (Word), R1 = Num Elementi
; Output: R0 = Count
; ----------------------------------------------------------------------------
        EXPORT contatore_inrange
contatore_inrange PROC
        MOV     r12, sp
        STMFD   sp!, {r4-r8,r10-r11,lr}
        
        LDR     R2, [R0]            ; R2 = VETT[0] (Limite Inf)
        
        SUB     R10, R1, #1         ; Indice ultimo (N-1)
        LSL     R11, R10, #2        ; Offset ultimo byte (N-1)*4
        LDR     R3, [R0, R11]       ; R3 = VETT[N-1] (Limite Sup)
        
        MOV     R4, #4              ; Inizia dal secondo elemento (offset 4)
        MOV     R5, #0              ; Count = 0
        
loop3   CMP     R4, R11             ; Se siamo arrivati all'ultimo elemento
        BGE     end_range           ; Esci (esclude l'ultimo dal conteggio)

        LDR     R6, [R0, R4]        ; Carica valore corrente
        
        CMP     R6, R2              ; Valore > Primo?
        BLE     skip_rg
        
        CMP     R6, R3              ; Valore < Ultimo?
        BGE     skip_rg
        
        ADD     R5, R5, #1          ; Count++
        
skip_rg ADD     R4, R4, #4          ; i++ (word step)
        B       loop3

end_range
        MOV     R0, R5              ; Return count
        LDMFD   sp!, {r4-r8,r10-r11,pc}
        ENDP

; ----------------------------------------------------------------------------
; Function: my_division
; Descrizione: Wrapper per divisione float.
; Input:  R0 -> Puntatore a float A, R1 -> Puntatore a float B
; Output: R0 = Risultato float
; ----------------------------------------------------------------------------
        EXPORT  my_division 
my_division PROC
        PUSH    {r4-r7,LR}
        
        LDR     R2, [R0]            ; Dereferenzia puntatore A
        LDR     R3, [R1]            ; Dereferenzia puntatore B
        MOV     R0, R2              ; Metti valori nei registri per la call
        MOV     R1, R3
        
        BL      __aeabi_fdiv        ; Chiama libreria C
        
        POP     {R4-R7, PC} 
        ENDP

; ----------------------------------------------------------------------------
; Function: count_negative_and_odd
; Descrizione: Conta numeri negativi E dispari.
; Input:  R0 = Vettore (Word), R1 = Num Elementi
; Output: R0 = Count
; ----------------------------------------------------------------------------
        EXPORT count_negative_and_odd
count_negative_and_odd PROC
        MOV     r12, sp
        STMFD   sp!, {r4-r8,r10-r11,lr}

        MOV     R2, #0              ; Offset i = 0
        LSL     R1, R1, #2          ; Converti N in N*4 (dimensione byte)
        MOV     R3, #0              ; Count = 0
        
loopodd LDR     R4, [R0, R2]        ; Carica Word
        
        CMP     R4, #0              ; Check Negativo
        BGE     next_odd            ; Se >= 0, salta
        
        TST     R4, #1              ; Check Dispari (Test LSB)
        BEQ     next_odd            ; Se 0 (Pari), salta
        
        ADD     R3, R3, #1          ; Trovato! Count++
        
next_odd            
        ADD     R2, R2, #4          ; i++
        CMP     R2, R1
        BNE     loopodd

        MOV     R0, R3
        LDMFD   sp!, {r4-r8,r10-r11,pc}
        ENDP

; ----------------------------------------------------------------------------
; Function: find_max
; Descrizione: Trova il valore massimo in un array di Word (32bit signed).
; Input:  R0 = Vettore, R1 = Num Elementi
; Output: R0 = Valore Massimo
; ----------------------------------------------------------------------------
        EXPORT find_max
find_max PROC
        PUSH    {r4-r5, lr}
        
        LDR     r2, [r0]            ; Assumi Max = VETT[0]
        MOV     r3, #4              ; Offset parte da 4 (secondo elemento)
        LSL     r1, r1, #2          ; N * 4 byte
        
loop_max CMP    r3, r1              ; Finito?
        BGE     end_max
        
        LDR     r4, [r0, r3]        ; Carica VETT[i]
        CMP     r4, r2              ; Confronta con Max attuale
        MOVGT   r2, r4              ; Se VETT[i] > Max, aggiorna Max (GT = Signed Greater Than)
        
        ADD     r3, r3, #4          ; i++
        B       loop_max
        
end_max MOV     r0, r2              ; Return Max
        POP     {r4-r5, pc}
        ENDP

; ----------------------------------------------------------------------------
; Function: string_to_upper
; Descrizione: Converte una stringa in MAIUSCOLO (in-place).
; Input:  R0 = Indirizzo stringa
; Output: Nessuno (Modifica la memoria)
; ----------------------------------------------------------------------------
        EXPORT string_to_upper
string_to_upper PROC
        PUSH    {r4, lr}
        MOV     r1, r0              ; Copia puntatore
        
loop_upper LDRB r2, [r1]            ; Leggi carattere
        CMP     r2, #0              ; Fine stringa?
        BEQ     end_upper
        
        CMP     r2, #'a'            ; Se < 'a', non è minuscolo
        BLT     skip_upper
        CMP     r2, #'z'            ; Se > 'z', non è minuscolo
        BGT     skip_upper
        
        SUB     r2, r2, #32         ; Converti: 'a'(97) - 32 = 'A'(65)
        STRB    r2, [r1]            ; Scrivi indietro
        
skip_upper ADD  r1, r1, #1          ; Next char
        B       loop_upper
        
end_upper POP   {r4, pc}
        ENDP

; ----------------------------------------------------------------------------
; Function: count_set_bits
; Descrizione: Conta i bit a '1' in una word (Population Count).
; Input:  R0 = Valore
; Output: R0 = Numero bit a 1
; ----------------------------------------------------------------------------
        EXPORT count_set_bits
count_set_bits PROC
        PUSH    {r4, lr}
        MOV     r1, r0          
        MOV     r0, #0          ; Counter
        
loop_bits CMP   r1, #0          
        BEQ     end_bits
        
        TST     r1, #1          ; Test LSB
        ADDNE   r0, r0, #1      ; Se 1, incrementa
        LSR     r1, r1, #1      ; Shift destra
        B       loop_bits
        
end_bits POP    {r4, pc}
        ENDP

; ----------------------------------------------------------------------------
; Function: check_palindrome
; Descrizione: Verifica se una stringa è palindroma.
; Input:  R0 = Stringa, R1 = Lunghezza
; Output: R0 = 1 (Vero), 0 (Falso)
; ----------------------------------------------------------------------------
        EXPORT check_palindrome
check_palindrome PROC
        PUSH    {r4-r6, lr}
        ADD     r2, r0, r1      
        SUB     r2, r2, #1      ; R2 = Puntatore fine
        
loop_pal CMP    r0, r2          ; Incrociato?
        BGE     is_pal
        
        LDRB    r3, [r0], #1    ; Carica Head e avanza
        LDRB    r4, [r2], #-1   ; Carica Tail e arretra
        
        CMP     r3, r4
        BNE     not_pal
        B       loop_pal
        
is_pal  MOV     r0, #1
        B       end_pal
not_pal MOV     r0, #0

end_pal POP     {r4-r6, pc}
        ENDP

; ----------------------------------------------------------------------------
; Function: factorial_recursive
; Descrizione: Fattoriale ricorsivo N!
; Input:  R0 = N
; Output: R0 = Risultato
; ----------------------------------------------------------------------------
        EXPORT factorial_recursive
factorial_recursive PROC
        PUSH    {r4, lr}
        CMP     r0, #1          
        BLE     base_fact       ; Base case <= 1
        
        MOV     r4, r0          ; Salva N
        SUB     r0, r0, #1      ; N-1
        BL      factorial_recursive
        MUL     r0, r4, r0      ; N * Fact(N-1)
        B       end_fact
        
base_fact MOV   r0, #1
end_fact POP    {r4, pc}
        ENDP

; ----------------------------------------------------------------------------
; Function: my_atoi
; Descrizione: Stringa ASCII -> Intero ("123" -> 123)
; Input:  R0 = Stringa
; Output: R0 = Intero
; ----------------------------------------------------------------------------
        EXPORT my_atoi
my_atoi PROC
        PUSH    {r4, r5, lr}
        MOV     r1, r0          
        MOV     r0, #0          ; Accumulatore
        MOV     r3, #10         ; Base
        
loop_atoi LDRB  r2, [r1], #1    
        CMP     r2, #0          
        BEQ     end_atoi
        
        SUB     r2, r2, #'0'    ; ASCII to Int
        CMP     r2, #9          
        BHI     end_atoi        ; Se non è cifra, stop
        
        MUL     r0, r0, r3      ; Acc * 10
        ADD     r0, r0, r2      ; Acc + Cifra
        B       loop_atoi
        
end_atoi POP    {r4, r5, pc}
        ENDP

; ----------------------------------------------------------------------------
; Function: next_state (LSFR)
; Descrizione: Linear Feedback Shift Register step.
; Input:  R0 = State, R1 = Taps, R2 = *OutputBit
; Output: R0 = New State
; ----------------------------------------------------------------------------
        EXPORT next_state
next_state PROC
        PUSH    {R4-R8, R10-R11, LR}  

        ; *output_bit = current_state & 1
        AND     R3, R0, #1          
        STR     R3, [R2]            

        MOV     R6, R0              ; Copia stato
        MOV     R7, #0              ; Input bit
        MOV     R4, #0              ; i loop
        MOV     R5, #1              ; Maschera

loop_lsfr CMP   R4, #8              
        BEQ     loop_lsfr_end

        TST     R1, R5              ; Tap attivo?
        LSRNE   R8, R6, R4          
        EORNE   R7, R7, R8          ; XOR accumulo
        
        LSL     R5, R5, #1          
        ADD     R4, R4, #1          
        B       loop_lsfr

loop_lsfr_end
        AND     R7, R7, #1          ; LSB solo
        LSR     R0, R0, #1          ; Shift stato
        LSL     R7, R7, #7          ; Input bit MSB
        ORR     R0, R0, R7          ; Combina

        POP     {R4-R8, R10-R11, PC}
        ENDP

; -------------------------------------------------------------------
; Conta i bit a 1 (Brian Kernighan)
; Input:  R0 = numero
; Output: R0 = numero di bit a 1
; -------------------------------------------------------------------
		EXPORT brianKernighan
brianKernighan PROC
        STMFD sp!, {r4-r5, lr}
        MOV r4, #0          ; counter
loopBK
        CMP r0, #0
        BEQ endBK
        SUB r5, r0, #1
        AND r0, r0, r5
        ADD r4, r4, #1
        B loopBK
endBK   MOV r0, r4
        LDMFD sp!, {r4-r5, pc}
		ENDP

; -------------------------------------------------------------------
; Conta leading zeros
; Input:  R0 = numero
; Output: R0 = numero di zeri iniziali
; -------------------------------------------------------------------
	EXPORT count_leading_zero
count_leading_zero PROC
        STMFD sp!, {r4-r8, r10-r11, lr}
        CLZ R0, R0
        LDMFD sp!, {r4-r8, r10-r11, pc}
		ENDP

; -------------------------------------------------------------------
; Massimo / Minimo di un array
; Input:  R0 = puntatore array, R1 = dimensione (numero elementi)
; Output: R0 = massimo o minimo
; -------------------------------------------------------------------
		EXPORT get_max
get_max PROC
        STMFD sp!, {r4-r8,r10-r11, lr}
        LDR R6, [R0], #4
        SUBS R1, R1, #1
        BLE exitMax
loopMax
        LDR R4, [R0], #4
        CMP R4, R6
        MOVGT R6, R4
        SUBS R1, R1, #1
        BGT loopMax
exitMax MOV R0, R6
        LDMFD sp!, {r4-r8,r10-r11, pc}
		ENDP

		EXPORT get_min
get_min PROC
        STMFD sp!, {r4-r8,r10-r11, lr}
        LDR R6, [R0], #4
        SUBS R1, R1, #1
        BLE exitMin
loopMin
        LDR R4, [R0], #4
        CMP R4, R6
        MOVLT R6, R4
        SUBS R1, R1, #1
        BGT loopMin
exitMin MOV R0, R6
        LDMFD sp!, {r4-r8,r10-r11, pc}
		ENDP

; -------------------------------------------------------------------
; Verifica se array è monotono crescente
; Input: R0 = puntatore array, R1 = dimensione
; Output: R0 = 1 se vero, 0 se falso
; -------------------------------------------------------------------
		EXPORT is_monotonic_increasing
is_monotonic_increasing PROC
        STMFD sp!,{r4-r8,r10-r11, lr}
        CMP R1, #1
        BLE exitTrue
        LDR R4, [R0], #4
        SUBS R1, R1, #1
loopCheck
        LDR R5, [R0], #4
        CMP R4, R5
        MOVGT R0, #0
        BGT exitFalse
        MOV R4, R5
        SUBS R1, R1, #1
        BGT loopCheck
exitTrue MOV R0, #1
        B endFunc
exitFalse MOV R0, #0
endFunc LDMFD sp!,{r4-r8,r10-r11, pc}
		ENDP

; -------------------------------------------------------------------
; Complemento a 2 di 32-bit
; Input:  R0 = numero
; Output: R0 = complemento a 2
; -------------------------------------------------------------------
		EXPORT do_2_complement
do_2_complement PROC
        STMFD sp!,{r4-r8,r10-r11,lr}
        MVN R0, R0
        ADD R0, R0, #1
        LDMFD sp!,{r4-r8,r10-r11,pc}
		ENDP

; -------------------------------------------------------------------
; Complemento a 2 di 64-bit
; Input:  R0 = upper, R1 = lower
; Output: R0,R1 = risultato
; -------------------------------------------------------------------
		EXPORT do_2_complement_64
do_2_complement_64 PROC
        STMFD sp!,{r4-r8,r10-r11,lr}
        MVN R0, R0
        MVN R1, R1
        ADDS R1, R1, #1
        BVC noOverflow
        ADD R0, R0, #1
noOverflow LDMFD sp!,{r4-r8,r10-r11,pc}
		ENDP

; -------------------------------------------------------------------
; Verifica se un valore è in un intervallo
; Input: R0 = valore, R1 = min, R2 = max
; Output: R0 = 1 se in range, 0 altrimenti
; -------------------------------------------------------------------
		EXPORT value_is_in_a_range
value_is_in_a_range PROC
        STMFD sp!,{r4-r8,r10-r11, lr}
        CMP R0, R1
        BLO outOfRange
        CMP R0, R2
        BHI outOfRange
        MOV R0, #1
        B exitFuncV
outOfRange
        MOV R0, #0
exitFuncV
        LDMFD sp!,{r4-r8,r10-r11,pc}
		ENDP

; -------------------------------------------------------------------
; Controllo carattere minuscolo
; Input: R0 = ASCII char
; Output: R0 = 1 se minuscolo, 0 altrimenti
; -------------------------------------------------------------------
		EXPORT check_lowerCase
check_lowerCase PROC
        STMFD sp!,{r4-r8,r10-r11,lr}
        CMP R0, #'a'
        BLT nope
        CMP R0, #'z'
        BGT nope
        MOV R0, #1
        BX lr
nope MOV R0, #0
        LDMFD sp!,{r4-r8,r10-r11,pc}
		ENDP

; -------------------------------------------------------------------
; Controllo carattere maiuscolo
; Input: R0 = ASCII char
; Output: R0 = 1 se maiuscolo, 0 altrimenti
; -------------------------------------------------------------------
		EXPORT check_upperCase
check_upperCase PROC
        STMFD sp!,{r4-r8,r10-r11,lr}
        CMP R0, #'A'
        BLT nope2
        CMP R0, #'Z'
        BGT nope2
        MOV R0, #1
        BX lr
nope2 MOV R0, #0
        LDMFD sp!,{r4-r8,r10-r11,pc}
		ENDP
			
; -------------------------------------------------------------------
; Somma di tutti gli elementi dell'array
; Input:  R0 = puntatore array, R1 = numero elementi
; Output: R0 = somma
; -------------------------------------------------------------------
		EXPORT array_sum
array_sum PROC
        STMFD sp!, {r4-r8, lr}

        MOV R2, #0          ; somma totale
        MOV R3, #0          ; indice

loop_sum
        CMP R3, R1
        BGE exit_sum
        LDR R4, [R0, R3, LSL #2]
        ADD R2, R2, R4
        ADD R3, R3, #1
        B loop_sum

exit_sum
        MOV R0, R2
        LDMFD sp!, {r4-r8, pc}
		ENDP

; -------------------------------------------------------------------
; Media (average) di tutti gli elementi dell'array
; Input:  R0 = puntatore array, R1 = numero elementi
; Output: R0 = media (intera, divisione intera)
; -------------------------------------------------------------------
		EXPORT array_average
array_average PROC
        STMFD sp!, {r4-r8, lr}

        CMP R1, #0
        BEQ zero_avg         ; protezione divisione per zero

        MOV R2, #0          ; somma totale (accumulatore)
        MOV R3, #0          ; indice i = 0

loop_avg
        CMP R3, R1
        BGE exit_avg
        LDR R4, [R0, R3, LSL #2] ; Carica array[i] (word = 4 byte, LSL #2)
        ADD R2, R2, R4      ; somma += valore
        ADD R3, R3, #1      ; i++
        B loop_avg

exit_avg
        ; Utilizzo della divisione hardware del Cortex-M3
        ; Sintassi: UDIV Rd, Rn, Rm  -> Rd = Rn / Rm
        UDIV R0, R2, R1     ; R0 = Somma (R2) / Num_Elementi (R1)
        LDMFD sp!, {r4-r8, pc}

zero_avg
        MOV R0, #0
        LDMFD sp!, {r4-r8, pc}
		ENDP

; -------------------------------------------------------------------
; Massimo dell'array
; Input: R0 = puntatore array, R1 = dimensione
; Output: R0 = massimo
; -------------------------------------------------------------------
		EXPORT get_max_1
get_max_1 PROC
        STMFD sp!, {r4-r8,r10-r11, lr}
        LDR R6, [R0], #4
        SUBS R1, R1, #1
        BLE exitMax_1
loopMax_1
        LDR R4, [R0], #4
        CMP R4, R6
        MOVGT R6, R4
        SUBS R1, R1, #1
        BGT loopMax_1
exitMax_1 MOV R0, R6
        LDMFD sp!, {r4-r8,r10-r11, pc}
		ENDP

; -------------------------------------------------------------------
; Minimo dell'array
; Input: R0 = puntatore array, R1 = dimensione
; Output: R0 = minimo
; -------------------------------------------------------------------
		EXPORT get_min_1
get_min_1 PROC
        STMFD sp!, {r4-r8,r10-r11, lr}
        LDR R6, [R0], #4
        SUBS R1, R1, #1
        BLE exitMin_1
loopMin_1
        LDR R4, [R0], #4
        CMP R4, R6
        MOVLT R6, R4
        SUBS R1, R1, #1
        BGT loopMin_1
exitMin_1 MOV R0, R6
        LDMFD sp!, {r4-r8,r10-r11, pc}
		ENDP

; -------------------------------------------------------------------
; Verifica se array è monotono crescente
; Input: R0 = puntatore array, R1 = dimensione
; Output: R0 = 1 se vero, 0 se falso
; -------------------------------------------------------------------
		EXPORT is_monotonic_increasing_1
is_monotonic_increasing_1 PROC
        STMFD sp!,{r4-r8,r10-r11, lr}
        CMP R1, #1
        BLE exitTrue_1
        LDR R4, [R0], #4
        SUBS R1, R1, #1
loopCheck_1
        LDR R5, [R0], #4
        CMP R4, R5
        MOVGT R0, #0
        BGT exitFalse_1
        MOV R4, R5
        SUBS R1, R1, #1
        BGT loopCheck
exitTrue_1 MOV R0, #1
        B endFunc_1
exitFalse_1 MOV R0, #0
endFunc_1 LDMFD sp!,{r4-r8,r10-r11, pc}
		ENDP

; -------------------------------------------------------------------
; Conta bit a 1 di tutti gli elementi dell'array
; Input: R0 = puntatore array, R1 = dimensione
; Output: R0 = numero totale di bit a 1
; -------------------------------------------------------------------
		EXPORT array_count_bit1
array_count_bit1 PROC
        STMFD sp!,{r4-r8, lr}
        MOV R2, #0          ; totale bit a 1
        MOV R3, #0          ; indice
loop_count
        CMP R3, R1
        BGE exit_count
        LDR R4, [R0, R3, LSL #2]
        MOV R0, R4
        BL brianKernighan
        ADD R2, R2, R0
        ADD R3, R3, #1
        B loop_count
exit_count
        MOV R0, R2
        LDMFD sp!,{r4-r8, pc}
		ENDP

; ----------------------------------------------------------------------------
; Controllo se numero primo
; Input: R0 = numero
; Output: R0 = 1 se primo, 0 altrimenti
; ----------------------------------------------------------------------------
        EXPORT is_prime
is_prime PROC
        STMFD sp!, {r4-r7, lr}
        CMP R0, #2
        BLT not_prime         ; 0,1 non sono primi
        MOV R1, #2            ; divisore iniziale
loop_prime
        MUL R2, R1, R1
        CMP R2, R0
        BGT prime_yes         ; se R1*R1 > N, è primo
        UDIV R3, R0, R1
        MUL R3, R3, R1
        CMP R3, R0
        BEQ not_prime         ; divisibile, non primo
        ADD R1, R1, #1
        B loop_prime
prime_yes
        MOV R0, #1
        B end_prime
not_prime
        MOV R0, #0
end_prime
        LDMFD sp!, {r4-r7, pc}
		ENDP

; ----------------------------------------------------------------------------
; MCD (Euclide)
; Input: R0 = A, R1 = B
; Output: R0 = MCD
; ----------------------------------------------------------------------------
        EXPORT gcd
gcd PROC
        STMFD sp!, {r4-r7, lr}
loop_gcd
        CMP R1, #0
        BEQ end_gcd
        MOV R2, R0
        UDIV R3, R0, R1
        MUL R3, R3, R1
        SUB R0, R2, R3      ; R0 = A mod B
        MOV R2, R0
        MOV R0, R1
        MOV R1, R2
        B loop_gcd
end_gcd
        LDMFD sp!, {r4-r7, pc}
		ENDP

; ----------------------------------------------------------------------------
; MCM (Minimo comune multiplo)
; Input: R0 = A, R1 = B
; Output: R0 = MCM
; ----------------------------------------------------------------------------
        EXPORT lcm
lcm PROC
        STMFD sp!, {r4-r7, lr}
        MOV R2, R0           ; Salva A
        MOV R3, R1           ; Salva B
        BL gcd                ; R0 = MCD(A,B)
        ; MCM = (A*B)/MCD
        MUL R0, R2, R3
        UDIV R0, R0, R0      ; R0 = (A*B)/MCD
        LDMFD sp!, {r4-r7, pc}
		ENDP

; ----------------------------------------------------------------------------
; Potenza intera
; Input: R0 = base, R1 = esponente
; Output: R0 = base^esponente
; ----------------------------------------------------------------------------
        EXPORT int_power
int_power PROC
        STMFD sp!, {r4-r7, lr}
        MOV R2, #1           ; Accumulatore risultato
        CMP R1, #0
        BEQ end_power
loop_power
        MUL R2, R2, R0
        SUBS R1, R1, #1
        BGT loop_power
end_power
        MOV R0, R2
        LDMFD sp!, {r4-r7, pc}
		ENDP

; ----------------------------------------------------------------------------
; Verifica numero pari / dispari
; Input: R0 = numero
; Output: R0 = 1 se pari, 0 se dispari
; ----------------------------------------------------------------------------
        EXPORT is_even
is_even PROC
        STMFD sp!, {r4-r7, lr}
        AND R1, R0, #1
        CMP R1, #0
        MOVEQ R0, #1
        MOVNE R0, #0
        LDMFD sp!, {r4-r7, pc}
		ENDP

; ----------------------------------------------------------------------------
; Array find (ritorna indice primo match)
; Input: R0 = puntatore array (word), R1 = dimensione, R2 = valore da cercare
; Output: R0 = indice se trovato, 0xFFFFFFFF se non trovato
; ----------------------------------------------------------------------------
        EXPORT array_find
array_find PROC
        STMFD sp!, {r4-r7, lr}
        MOV R3, #0           ; indice
loop_find
        CMP R3, R1
        BGE not_found
        LDR R4, [R0, R3, LSL #2]
        CMP R4, R2
        BEQ found
        ADD R3, R3, #1
        B loop_find
found
        MOV R0, R3
        B end_find
not_found
        MOV R0, #0xFFFFFFFF
end_find
        LDMFD sp!, {r4-r7, pc}
		ENDP

; ----------------------------------------------------------------------------
; Array reverse (in-place)
; Input: R0 = puntatore array (word), R1 = dimensione
; Output: nessuno, modifica array in-place
; ----------------------------------------------------------------------------
        EXPORT array_reverse
array_reverse PROC
        STMFD sp!, {r4-r7, lr}
        MOV R2, #0            ; i = 0
        SUB R3, R1, #1        ; j = N-1
loop_rev
        CMP R2, R3
        BGE end_reverse
        LDR R4, [R0, R2, LSL #2]
        LDR R5, [R0, R3, LSL #2]
        STR R5, [R0, R2, LSL #2]
        STR R4, [R0, R3, LSL #2]
        ADD R2, R2, #1
        SUB R3, R3, #1
        B loop_rev
end_reverse
        LDMFD sp!, {r4-r7, pc}
		ENDP

; ----------------------------------------------------------------------------
; Controllo quadrato perfetto
; Input: R0 = numero
; Output: R0 = 1 se quadrato perfetto, 0 altrimenti
; ----------------------------------------------------------------------------
        EXPORT is_perfect_square
is_perfect_square PROC
        STMFD sp!, {r4-r7, lr}
        MOV R1, #0            ; i = 0
loop_square
        MUL R2, R1, R1
        CMP R2, R0
        BEQ square_yes
        CMP R2, R0
        BGT square_no
        ADD R1, R1, #1
        B loop_square
square_yes
        MOV R0, #1
        B end_square
square_no
        MOV R0, #0
end_square
        LDMFD sp!, {r4-r7, pc}
		ENDP

; ============================================================================
; SEZIONE DATI (VARIABILI GLOBALI ASSEMBLY)
; ============================================================================
        
        ; Area Costanti (ReadOnly)
        AREA    _data, DATA, READONLY, ALIGN=3
        EXPORT  my_vector 
        EXPORT  my_variable
my_vector       DCD -5, 0xFFFFFF01, 10, 20, -2, 33  ; Vettore di prova
my_variable     DCB 'A'                             ; Variabile char

        ; Area Variabili (ReadWrite)
        AREA    _data2, DATA, READWRITE, ALIGN=3
        EXPORT  my_space_4_vector
my_space_4_vector SPACE 11*22 ; Spazio vuoto allocato (Buffer)

        END