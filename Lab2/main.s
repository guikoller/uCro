; Exemplo.s
; Desenvolvido para a placa EK-TM4C1294XL
; Prof. Guilherme Peron
; 12/03/2018

; -------------------------------------------------------------------------------
        THUMB                        ; Instru��es do tipo Thumb-2
; -------------------------------------------------------------------------------
; Declara��es EQU - Defines
;<NOME>         EQU <VALOR>
; -------------------------------------------------------------------------------
; �rea de Dados - Declara��es de vari�veis
		AREA  DATA, ALIGN=2
		; Se alguma vari�vel for chamada em outro arquivo
		;EXPORT  <var> [DATA,SIZE=<tam>]   ; Permite chamar a vari�vel <var> a 
		                                   ; partir de outro arquivo
;<var>	SPACE <tam>                        ; Declara uma vari�vel de nome <var>
                                           ; de <tam> bytes a partir da primeira 
                                           ; posi��o da RAM	

	Tab_1 SPACE 0x4
	Tab_2 SPACE 0x4
	Tab_3 SPACE 0x4
	Tab_4 SPACE 0x4
	Tab_5 SPACE 0x4
	Tab_6 SPACE 0x4
	Tab_7 SPACE 0x4
	Tab_8 SPACE 0x4
	Tab_9 SPACE 0x4
	
; -------------------------------------------------------------------------------
; �rea de C�digo - Tudo abaixo da diretiva a seguir ser� armazenado na mem�ria de 
;                  c�digo
        AREA    |.text|, CODE, READONLY, ALIGN=2

		; Se alguma fun��o do arquivo for chamada em outro arquivo	
        EXPORT Start                ; Permite chamar a fun��o Start a partir de 
			                        ; outro arquivo. No caso startup.s
									
		; Se chamar alguma fun��o externa	
        ;IMPORT <func>              ; Permite chamar dentro deste arquivo uma 
									; fun��o <func>
									
		IMPORT  PLL_Init
		IMPORT  SysTick_Init
		IMPORT  SysTick_Wait1ms			
		IMPORT  GPIO_Init

		IMPORT LCD_init
		IMPORT LCD_printArrayInLcd
		IMPORT LCD_ResetLCD
		IMPORT LCD_PulaCursorSegundaLinha
		IMPORT MKEYBOARD_readKeyboard
		IMPORT blinkLEDs

; -------------------------------------------------------------------------------
; Funcao main()
Start
	BL PLL_Init                  ;Chama a subrotina para alterar o clock do microcontrolador para 80MHz
	BL SysTick_Init              ;Chama a subrotina para inicializar o SysTick
	BL GPIO_Init
	BL LCD_init                  ;Chama a subrotina que inicializa os GPIO
	BL InitilizeVars
;--------------------------------------------------------------------------------
MainLoop

	MSG_TABUADA		DCB      "Tabuada do ", 0
	IGUAL 			DCB		 " = "
	VEZES 			DCB		 " x "

    ALIGN                           ; garante que o fim da se��o est� alinhada 
    END                             ; fim do arquivo
