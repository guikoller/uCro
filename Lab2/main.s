; Exemplo.s
; Desenvolvido para a placa EK-TM4C1294XL
; Prof. Guilherme Peron
; 12/03/2018

; -------------------------------------------------------------------------------
        THUMB                        ; Instruções do tipo Thumb-2
; -------------------------------------------------------------------------------
; Declarações EQU - Defines
;<NOME>         EQU <VALOR>
; -------------------------------------------------------------------------------
; Área de Dados - Declarações de variáveis
		AREA  DATA, ALIGN=2
		; Se alguma variável for chamada em outro arquivo
		;EXPORT  <var> [DATA,SIZE=<tam>]   ; Permite chamar a variável <var> a 
		                                   ; partir de outro arquivo
;<var>	SPACE <tam>                        ; Declara uma variável de nome <var>
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
	
; -------------------------------------------------------------------------------
; Área de Código - Tudo abaixo da diretiva a seguir será armazenado na memória de 
;                  código
        AREA    |.text|, CODE, READONLY, ALIGN=2

		; Se alguma função do arquivo for chamada em outro arquivo	
        EXPORT Start                ; Permite chamar a função Start a partir de 
			                        ; outro arquivo. No caso startup.s
									
		; Se chamar alguma função externa	
        ;IMPORT <func>              ; Permite chamar dentro deste arquivo uma 
									; função <func>
									
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

    ALIGN                           ; garante que o fim da seção está alinhada 
    END                             ; fim do arquivo
