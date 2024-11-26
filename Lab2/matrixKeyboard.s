;matrix_keyboard.s
; Desenvolvido para a placa EK-TM4C1294XL
; Codigo que apresenta algumas funcionalidades:

; -------------------------------------------------------------------------------
	THUMB	; instrucoes do tipo Thumb-2
; -------------------------------------------------------------------------------
; Declaracoes EQU - Defines
; ========================
; Definicoes dos Registradores Gerais
SYSCTL_RCGCGPIO_R   EQU	0x400FE608
SYSCTL_PRGPIO_R     EQU 0x400FEA08

; ========================
; Definicoes dos Ports

;PORT L
GPIO_PORTL				EQU 2_10000000000
GPIO_PORTL_DATA_BITS_R  EQU 0x40062000
GPIO_PORTL_DATA_R       EQU 0x400623FC
GPIO_PORTL_DIR_R        EQU 0x40062400
GPIO_PORTL_IS_R         EQU 0x40062404
GPIO_PORTL_IBE_R        EQU 0x40062408
GPIO_PORTL_IEV_R        EQU 0x4006240C
GPIO_PORTL_IM_R         EQU 0x40062410
GPIO_PORTL_RIS_R        EQU 0x40062414
GPIO_PORTL_MIS_R        EQU 0x40062418
GPIO_PORTL_ICR_R        EQU 0x4006241C
GPIO_PORTL_AFSEL_R      EQU 0x40062420
GPIO_PORTL_DR2R_R       EQU 0x40062500
GPIO_PORTL_DR4R_R       EQU 0x40062504
GPIO_PORTL_DR8R_R       EQU 0x40062508
GPIO_PORTL_ODR_R        EQU 0x4006250C
GPIO_PORTL_PUR_R        EQU 0x40062510
GPIO_PORTL_PDR_R        EQU 0x40062514
GPIO_PORTL_SLR_R        EQU 0x40062518
GPIO_PORTL_DEN_R        EQU 0x4006251C
GPIO_PORTL_LOCK_R       EQU 0x40062520
GPIO_PORTL_CR_R         EQU 0x40062524
GPIO_PORTL_AMSEL_R      EQU 0x40062528
GPIO_PORTL_PCTL_R       EQU 0x4006252C
GPIO_PORTL_ADCCTL_R     EQU 0x40062530
GPIO_PORTL_DMACTL_R     EQU 0x40062534
GPIO_PORTL_SI_R         EQU 0x40062538
GPIO_PORTL_DR12R_R      EQU 0x4006253C
GPIO_PORTL_WAKEPEN_R    EQU 0x40062540
GPIO_PORTL_WAKELVL_R    EQU 0x40062544
GPIO_PORTL_WAKESTAT_R   EQU 0x40062548
GPIO_PORTL_PP_R         EQU 0x40062FC0
GPIO_PORTL_PC_R         EQU 0x40062FC4

;PORT M
GPIO_PORTM				EQU 2_1000000000000
GPIO_PORTM_DATA_BITS_R  EQU 0x40063000
GPIO_PORTM_DATA_R       EQU 0x400633FC
GPIO_PORTM_DIR_R        EQU 0x40063400
GPIO_PORTM_IS_R         EQU 0x40063404
GPIO_PORTM_IBE_R        EQU 0x40063408
GPIO_PORTM_IEV_R        EQU 0x4006340C
GPIO_PORTM_IM_R         EQU 0x40063410
GPIO_PORTM_RIS_R        EQU 0x40063414
GPIO_PORTM_MIS_R        EQU 0x40063418
GPIO_PORTM_ICR_R        EQU 0x4006341C
GPIO_PORTM_AFSEL_R      EQU 0x40063420
GPIO_PORTM_DR2R_R       EQU 0x40063500
GPIO_PORTM_DR4R_R       EQU 0x40063504
GPIO_PORTM_DR8R_R       EQU 0x40063508
GPIO_PORTM_ODR_R        EQU 0x4006350C
GPIO_PORTM_PUR_R        EQU 0x40063510
GPIO_PORTM_PDR_R        EQU 0x40063514
GPIO_PORTM_SLR_R        EQU 0x40063518
GPIO_PORTM_DEN_R        EQU 0x4006351C
GPIO_PORTM_LOCK_R       EQU 0x40063520
GPIO_PORTM_CR_R         EQU 0x40063524
GPIO_PORTM_AMSEL_R      EQU 0x40063528
GPIO_PORTM_PCTL_R       EQU 0x4006352C
GPIO_PORTM_ADCCTL_R     EQU 0x40063530
GPIO_PORTM_DMACTL_R     EQU 0x40063534
GPIO_PORTM_SI_R         EQU 0x40063538
GPIO_PORTM_DR12R_R      EQU 0x4006353C
GPIO_PORTM_WAKEPEN_R    EQU 0x40063540
GPIO_PORTM_WAKELVL_R    EQU 0x40063544
GPIO_PORTM_WAKESTAT_R   EQU 0x40063548
GPIO_PORTM_PP_R         EQU 0x40063FC0
GPIO_PORTM_PC_R         EQU 0x40063FC4

; -------------------------------------------------------------------------------
; Area de Codigo - Tudo abaixo da diretiva a seguir sera armazenado na memoria de 
; codigo
        AREA    |.text|, CODE, READONLY, ALIGN=2

		; Se alguma funcao do arquivo for chamada em outro arquivo	
		EXPORT MKBOARD_GPIOinit
		EXPORT MKEYBOARD_readKeyboard
		
		IMPORT SysTick_Wait1ms

;--------------------------------------------------------------------------------
; Funcao MKBOARD_init
; Parametro de entrada: Nao tem
; Parametro de saida: Nao tem
MKBOARD_GPIOinit
;=====================
	; VER arquivo LCD.S
	; 1. Ativar o clock para a porta setando o bit correspondente no registrador RCGCGPIO,
	LDR	R0, =SYSCTL_RCGCGPIO_R  ;Carrega o endereao do registrador RCGCGPIO
	LDR R1, [R0]
	ORR	R1, #GPIO_PORTL         ; teclado - PINOS LINHAS 
	ORR R1, #GPIO_PORTM			; teclado - PINOS COLUNAS
   STR R1, [R0]				;Move para a memoria os bits das portas no endereao do RCGCGPIO

	; verificar no PRGPIO se a porta esta pronta para uso.
	LDR R0, =SYSCTL_PRGPIO_R	;Carrega o endereao do PRGPIO para esperar os GPIO ficarem prontos
Espera_Porta  
	LDR R1, [R0]				;Le da memoria o conteado do endereao do registrador
	MOV R2, #GPIO_PORTL
	ORR R2, #GPIO_PORTM
	AND R1, R1, R2		;seleciona apenas os bits das portas referentes
	TST R1, R2			;compara se os bits estao iguais
	BEQ Espera_Porta	;Se o flag Z=1, volta para o laao. Senao continua executando
   
	; 2. Limpar o AMSEL para desabilitar a analagica
	LDR R0, =GPIO_PORTL_AMSEL_R
	LDR R1, [R0]
	BIC R1, #0xF	; L0 a L3 = 0 : desabilita analogica
	STR R1, [R0]
	
	LDR R0, =GPIO_PORTM_AMSEL_R
	LDR R1, [R0]
	BIC R1, #0xF0	; M4 a M7 = 0 : desabilita analogica
	STR R1, [R0]
   
	; 3. Limpar PCTL para selecionar o GPIO
	LDR R0, =GPIO_PORTL_PCTL_R
	LDR R1, [R0]
	BIC R1, R1, #0xF	;L0 a L3 = 0: seleciona modo GPIO
	STR R1, [R0]
	
	LDR R0, =GPIO_PORTM_PCTL_R
	LDR R1, [R0]
	BIC R1, R1, #0xF0	;M4 a M7 = 0: seleciona modo GPIO
	STR R1, [R0]

	; 4. DIR para 0: input (BIC), 1: output (ORR)
   LDR R0, =GPIO_PORTL_DIR_R
	LDR R1, [R0]
	BIC R1, R1, #0xF	;L0 a L3 = 0: INPUT
   STR R1, [R0]
	
   LDR R0, =GPIO_PORTM_DIR_R
	LDR R1, [R0]
	ORR	R1, R1, #0xF0	;M4 a M7 = 1: OUTPUT
   STR R1, [R0]
	
	; 5. Limpar os bits AFSEL para 0 para selecionar GPIO sem funcao alternativa
	LDR R0, =GPIO_PORTL_AFSEL_R
	LDR R1, [R0]
	BIC R1, R1, #0xF	;L0 a L3 = 0: sem funcao alternativa
	STR R1, [R0]
	LDR R0, =GPIO_PORTM_AFSEL_R
	LDR R1, [R0]
	BIC R1, R1, #0xF0	;M4 a M7 = 0: sem funcao alternativa
	STR R1, [R0]
	
	; 6. Setar os bits de DEN para habilitar I/O digital
	LDR R0, =GPIO_PORTL_DEN_R	;carrega o endereao do DEN
	LDR R1, [R0]
	ORR R1, R1, #0xF	;L0 a L3 = 1: habilita I/O digital
	STR R1, [R0]
   
	LDR R0, =GPIO_PORTM_DEN_R
	LDR R1, [R0]
	ORR R1, R1, #0xF0	;M4 a M7 = 1: habilita I/O digital
	STR R1, [R0]
   
	; 7. Para habilitar resistor de pull-up interno, setar PUR para 1
	LDR R0, =GPIO_PORTL_PUR_R	;Carrega o endereao do PUR
	LDR R1, [R0]
	ORR R1, R1, #0xF	;L0 a L3 = 1: habilita funcionalidade digital de resistor de pull-up
   STR R1, [R0]

	BX  LR ;return

; -------------------------------------------------------------------------------
; Function to read button input from the matrix keyboard(4x4)
; input: none
; output: R0 -> button pressed
MKEYBOARD_readKeyboard
	; Bounce do teclado
	MOV R0, #300
	PUSH { LR }	
	BL SysTick_Wait1ms
	POP { LR }
	MOV R4, #4
verifica_teclado
	MOV R0, R4
	PUSH { LR }
	BL ler_coluna
	BL ler_porta_L
	POP { LR }
	CMP R2, #0xF
	BEQ depois_de_verificar_teclado
tecla_pressionada
	MOV R3, #1
	LSL R3, R4
	ORR R0, R2, R3
	B decideValueToReturn
depois_de_verificar_teclado
	CMP R4, #7
	ITT EQ
		MOVEQ R0, #0
		BEQ readKeyboardEnd
	ADD R4, #1
	B verifica_teclado
readKeyboardEnd
	BX LR

decideValueToReturn
	CMP R0, #2_00011110 ; C1, L1
	ITT EQ
		MOVEQ R0, #'1'
		BEQ readKeyboardEnd
	CMP R0, #2_00011101; C1, L2
	ITT EQ
		MOVEQ R0, #'4'
		BEQ readKeyboardEnd
	CMP R0, #2_00011011; C1, L3
	ITT EQ
		MOVEQ R0, #'7'
		BEQ readKeyboardEnd
	CMP R0, #2_00010111; C1, L4
	ITT EQ
		MOVEQ R0, #'*'
		BEQ readKeyboardEnd
	CMP R0, #2_00101110; C2, L1
	ITT EQ
		MOVEQ R0, #'2'
		BEQ readKeyboardEnd
	CMP R0, #2_00101101; C2, L2
	ITT EQ
		MOVEQ R0, #'5'
		BEQ readKeyboardEnd
	CMP R0, #2_00101011; C2, L3
	ITT EQ
		MOVEQ R0, #'8'
		BEQ readKeyboardEnd
	CMP R0, #2_00100111; C2, L4
	ITT EQ
		MOVEQ R0, #'0'
		BEQ readKeyboardEnd
	CMP R0, #2_01001110; C3, L1
	ITT EQ
		MOVEQ R0, #'3'
		BEQ readKeyboardEnd
	CMP R0, #2_01001101; C3, L2
	ITT EQ
		MOVEQ R0, #'6'
		BEQ readKeyboardEnd
	CMP R0, #2_01001011; C3, L3
	ITT EQ
		MOVEQ R0, #'9'
		BEQ readKeyboardEnd
	CMP R0, #2_01000111; C3, L4
	ITT EQ
		MOVEQ R0, #'#'
		BEQ readKeyboardEnd
	CMP R0, #2_10001110; C4, L1
	ITT EQ
		MOVEQ R0, #'A'
		BEQ readKeyboardEnd
	CMP R0, #2_10001101; C4, L2
	ITT EQ
		MOVEQ R0, #'B'
		BEQ readKeyboardEnd
	CMP R0, #2_10001011; C4, L3
	ITT EQ
		MOVEQ R0, #'C'
		BEQ readKeyboardEnd
	MOV R0, #'D'
	B readKeyboardEnd
; -------------------------------------------------------------------------------
; Funcao ler_coluna
; Parametro de entrada: R0 -> numero da coluna que se quer ler - de 4 a 7
; Parametro de saida: Nao tem
ler_coluna 
	LDR R1, =GPIO_PORTM_DIR_R
	
	; R3 vai ter como bit 1 o bit na posicao R0
	MOV R3, #1
	LSL R3, R0
	LDR R2, [R1]
	AND R2, #0x0F
	ORR R3, R2

	STR R3, [R1] ; Escreve o novo valor da porta 0 ENTRADA 1 SAIDA
	
	; Escreve 0 na coluna escolhida
	LDR R1, =GPIO_PORTM_DATA_R
	LDR R2, [R1]
	AND R2, #0x0F
	MOV R3, #1
	LSL R3, R0
	EOR R3, R3, #0xF0
	AND R3, #0xF0
	ORR R2, R3 ; R2 = R2 & 0x0F | R3 & 0xF0
	STR R2, [R1]

	PUSH{LR, R0}
	MOV R0, #10
	BL SysTick_Wait1ms
	POP{R0, LR}
	
	BX LR

; -------------------------------------------------------------------------------
; Funcao ler_porta_L - Le o valor de PL3-PL0
; Parametro de entrada: Nao tem
; Parametro de saida: Nao tem
ler_porta_L
	LDR R1, =GPIO_PORTL_DATA_R
	LDR R2, [R1] ; 
	AND R2, #0x0F
	
	BX LR


; -------------------------------------------------------------------------------
; fim do arquivo
	ALIGN                           ; garante que o fim da se??o est? alinhada 
    END                             ; fim do arquivo