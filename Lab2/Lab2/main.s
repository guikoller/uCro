; Exemplo.s
; Desenvolvido para a placa EK-TM4C1294XL
; Prof. Guilherme Peron
; 12/03/2018

; -------------------------------------------------------------------------------
        THUMB                        ; Instruções do tipo Thumb-2
; -------------------------------------------------------------------------------
; Declarações EQU - Defines
STATE_INIT        EQU 0
STATE_LE_TECLADO  EQU 1
STATE_IMPRIME_LCD EQU 2
STATE_RESET_TAB   EQU 3

; -------------------------------------------------------------------------------
; Área de Dados - Declarações de variáveis
        AREA  DATA, ALIGN=2
        ; Se alguma variável for chamada em outro arquivo
        ; EXPORT  <var> [DATA,SIZE=<tam>]   ; Permite chamar a variável <var> a 
                                           ; partir de outro arquivo
; <var>  SPACE <tam>                        ; Declara uma variável de nome <var>
                                           ; de <tam> bytes a partir da primeira 
                                           ; posição da RAM    

Tab_1 SPACE 0x4
Tab_2 SPACE 0x4
Tab_3 SPACE 0x4
Tab_4 SPACE 0x4
Tab_5 SPACE 0x4
Tab_6 SPACE 0x4
Tab_7 SPACE 0x4
Tab_8 SPACE 0x4
Tab_9 SPACE 0x4

current_state SPACE 0x4
current_number SPACE 0x4
current_multiplier SPACE 0x4

; -------------------------------------------------------------------------------
; Área de Código - Tudo abaixo da diretiva a seguir será armazenado na memória de 
;                  código
        AREA    |.text|, CODE, READONLY, ALIGN=2

        ; Se alguma função do arquivo for chamada em outro arquivo    
        EXPORT Start                ; Permite chamar a função Start a partir de 
                                    ; outro arquivo. No caso startup.s
                                    
        ; Se chamar alguma função externa    
        ; IMPORT <func>              ; Permite chamar dentro deste arquivo uma 
                                    ; função <func>
                                    
        IMPORT  PLL_Init
        IMPORT  SysTick_Init
        IMPORT  SysTick_Wait1ms
        IMPORT  SysTick_Wait            
        IMPORT  GPIO_Init
        IMPORT  LCD_init
        IMPORT  LCD_printArrayInLcd
        IMPORT  LCD_ResetLCD
        IMPORT  LCD_PulaCursorSegundaLinha
        IMPORT  LCD_SetCursorPos
        IMPORT  MKEYBOARD_readKeyboard
        IMPORT  envia_dado_lcd

Start
        ; Inicialização
        BL      PLL_Init
        BL      SysTick_Init
        BL      GPIO_Init
        BL      LCD_init
        BL      LCD_ResetLCD

        ; Inicializa variáveis
        LDR     R0, =current_state
        MOVS    R1, #STATE_INIT
        STR     R1, [R0]

MainLoop
        ; Máquina de estados
        LDR     R0, =current_state
        LDR     R1, [R0]
        CMP     R1, #STATE_INIT
        BEQ     InitilizeVars
        CMP     R1, #STATE_LE_TECLADO
        BEQ     leTeclado
        CMP     R1, #STATE_IMPRIME_LCD
        BEQ     imprimeLCD
        CMP     R1, #STATE_RESET_TAB
        BEQ     reset_tabuada
        B       MainLoop

InitilizeVars
        ; Inicializa variáveis
        LDR     R0, =Tab_1
        MOVS    R1, #0
        STR     R1, [R0]
        
        LDR     R0, =Tab_2
        STR     R1, [R0]
        
        LDR     R0, =Tab_3
        STR     R1, [R0]
        
        LDR     R0, =Tab_4
        STR     R1, [R0]
        
        LDR     R0, =Tab_5
        STR     R1, [R0]
        
        LDR     R0, =Tab_6
        STR     R1, [R0]
        
        LDR     R0, =Tab_7
        STR     R1, [R0]
        
        LDR     R0, =Tab_8
        STR     R1, [R0]
        
        LDR     R0, =Tab_9
        STR     R1, [R0]

        ; Inicializa current_number e current_multiplier
        LDR     R0, =current_number
        MOVS    R1, #0
        STR     R1, [R0]

        LDR     R0, =current_multiplier
        STR     R1, [R0]

        ; Exibe mensagem padrão "Tabuada do"
        BL      LCD_ResetLCD
        LDR     R0, =MSG_TABUADA
        MOVS    R1, #11
        BL      LCD_printArrayInLcd

        ; Muda para o estado de leitura do teclado
        LDR     R0, =current_state
        MOVS    R1, #STATE_LE_TECLADO
        STR     R1, [R0]
        B       MainLoop

leTeclado
        ; Lê entrada do teclado
        BL      MKEYBOARD_readKeyboard
        CMP     R0, #0
        BEQ     MainLoop

        ; Converte ASCII para valor numérico
        SUBS    R0, R0, #0x30

        ; Atualiza número atual
        LDR     R1, =current_number
        STR     R0, [R1]

        ; Incrementa multiplicador
        LDR     R1, =current_multiplier
        LDR     R2, [R1]
        ADDS    R2, R2, #1
        STR     R2, [R1]

        ; Verifica se o multiplicador é maior que 9
        CMP     R2, #10
        BGE     reset_tabuada

        ; Muda para o estado de imprimir no LCD
        LDR     R0, =current_state
        MOVS    R1, #STATE_IMPRIME_LCD
        STR     R1, [R0]
        B       MainLoop

imprimeLCD

		; Exibe "Tabuada do n"
		BL      LCD_ResetLCD	
		LDR     R0, =MSG_TABUADA
		MOVS    R1, #11
		BL      LCD_printArrayInLcd
		
		LDR     R1, =current_number
		LDR     R2, [R1]
		ADD     R1, R2, #0x30  ; Converte valor numérico para ASCII
		BL      envia_dado_lcd
		BL      LCD_PulaCursorSegundaLinha

		; Exibe "n x m = resultado"
		LDR     R1, =current_number
		LDR     R2, [R1]
		LDR     R1, =current_multiplier
		LDR     R3, [R1]
		MUL     R4, R2, R3

		; Exibe número atual
		ADD     R1, R2, #0x30  ; Converte valor numérico para ASCII
		BL      envia_dado_lcd

		; Exibe "x"
		LDR     R0, =VEZES
		MOVS    R1, #1
		BL      LCD_printArrayInLcd

		; Exibe multiplicador
		ADD     R1, R3, #0x30  ; Converte valor numérico para ASCII
		BL      envia_dado_lcd

		; Exibe "="
		LDR     R0, =IGUAL
		MOVS    R1, #1
		BL      LCD_printArrayInLcd

		; Exibe resultado
		MOVS    R5, R4
		MOVS    R6, #10
		UDIV    R7, R5, R6      ; Divide por 10 para obter a dezena
		MLS     R5, R7, R6, R5  ; Subtrai a dezena para obter a unidade
		ADD     R1, R7, #0x30   ; Converte dezena para ASCII
		BL      envia_dado_lcd
		ADD     R1, R5, #0x30   ; Converte unidade para ASCII
		BL      envia_dado_lcd

		; Muda para o estado de leitura do teclado
		LDR     R0, =current_state
		MOVS    R1, #STATE_LE_TECLADO
		STR     R1, [R0]
		B       MainLoop

reset_tabuada
        ; Reseta multiplicador
        LDR     R1, =current_multiplier
        MOVS    R2, #0
        STR     R2, [R1]

        ; Atualiza número atual
        LDR     R1, =current_number
        STR     R0, [R1]

        ; Muda para o estado de imprimir no LCD
        LDR     R0, =current_state
        MOVS    R1, #STATE_IMPRIME_LCD
        STR     R1, [R0]
        B       MainLoop

MSG_TABUADA     DCB      "Tabuada do ", 0
IGUAL           DCB      "=", 0
VEZES           DCB      "x", 0
NUMBERS         DCB      0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0

        ALIGN                           ; garante que o fim da seção está alinhada 
        END                             ; fim do arquivo