;lcd.s
; Desenvolvido para a placa EK-TM4C1294XL
; Codigo que apresenta algumas funcionalidades:
; LCD_init
; LCD_reset
; LCD_command
; LCD_write_data

; -------------------------------------------------------------------------------
   THUMB   ; instrucoes do tipo Thumb-2
; -------------------------------------------------------------------------------
; Declaracoes EQU - Defines
; ========================
; Definicoes dos Registradores Gerais
SYSCTL_RCGCGPIO_R    EQU   0x400FE608
SYSCTL_PRGPIO_R       EQU    0x400FEA08
; ========================
; Definicoes dos Ports

; PORT K
GPIO_PORTK              EQU 2_1000000000
GPIO_PORTK_DATA_BITS_R  EQU 0x40061000
GPIO_PORTK_DATA_R       EQU 0x400613FC
GPIO_PORTK_DIR_R        EQU 0x40061400
GPIO_PORTK_IS_R         EQU 0x40061404
GPIO_PORTK_IBE_R        EQU 0x40061408
GPIO_PORTK_IEV_R        EQU 0x4006140C
GPIO_PORTK_IM_R         EQU 0x40061410
GPIO_PORTK_RIS_R        EQU 0x40061414
GPIO_PORTK_MIS_R        EQU 0x40061418
GPIO_PORTK_ICR_R        EQU 0x4006141C
GPIO_PORTK_AFSEL_R      EQU 0x40061420
GPIO_PORTK_DR2R_R       EQU 0x40061500
GPIO_PORTK_DR4R_R       EQU 0x40061504
GPIO_PORTK_DR8R_R       EQU 0x40061508
GPIO_PORTK_ODR_R        EQU 0x4006150C
GPIO_PORTK_PUR_R        EQU 0x40061510
GPIO_PORTK_PDR_R        EQU 0x40061514
GPIO_PORTK_SLR_R        EQU 0x40061518
GPIO_PORTK_DEN_R        EQU 0x4006151C
GPIO_PORTK_LOCK_R       EQU 0x40061520
GPIO_PORTK_CR_R         EQU 0x40061524
GPIO_PORTK_AMSEL_R      EQU 0x40061528
GPIO_PORTK_PCTL_R       EQU 0x4006152C
GPIO_PORTK_ADCCTL_R     EQU 0x40061530
GPIO_PORTK_DMACTL_R     EQU 0x40061534
GPIO_PORTK_SI_R         EQU 0x40061538
GPIO_PORTK_DR12R_R      EQU 0x4006153C
GPIO_PORTK_WAKEPEN_R    EQU 0x40061540
GPIO_PORTK_WAKELVL_R    EQU 0x40061544
GPIO_PORTK_WAKESTAT_R   EQU 0x40061548
GPIO_PORTK_PP_R         EQU 0x40061FC0
GPIO_PORTK_PC_R         EQU 0x40061FC4

; PORT M
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
GPIO_PORTM              EQU 2_100000000000

; -------------------------------------------------------------------------------
; Area de Codigo - Tudo abaixo da diretiva a seguir sera armazenado na memoria de 
; codigo
        AREA    |.text|, CODE, READONLY, ALIGN=2

      ; Se alguma funcao do arquivo for chamada em outro arquivo   
      EXPORT LCD_init
      EXPORT LCD_GPIOinit
      EXPORT LCD_PulaCursorSegundaLinha
      EXPORT LCD_ResetLCD
      EXPORT LCD_printArrayInLcd
      EXPORT LCD_SetCursorPos
      
      IMPORT SysTick_Wait1us
      IMPORT SysTick_Wait1ms

;--------------------------------------------------------------------------------
; Funcao LCD_GPIOinit
; Parametro de entrada: Nao tem
; Parametro de saida: Nao tem
LCD_GPIOinit
   ; 1. Ativar o clock para a porta setando o bit correspondente no registrador RCGCGPIO,
   LDR   R0, =SYSCTL_RCGCGPIO_R   ;carrega o endereco do registrador RCGCGPIO
   LDR R1, [R0]            ;carrega dado do registrador
   ORR R1, #GPIO_PORTK         ;seta apenas pinos da porta dos PINOS DE DADOS LCD 
   ORR   R1, #GPIO_PORTM         ;seta apenas pinos da porta dos PINOS DE COMANDO LCD 
   STR R1, [R0]            ;move para a memoria os bits das portas no endereco do RCGCGPIO

   ; verificar no PRGPIO se a porta esta pronta para uso.
   LDR R0, =SYSCTL_PRGPIO_R   ;carrega o endereco do PRGPIO para esperar os GPIO ficarem prontos
Espera_porta
   LDR   R1, [R0]         ;le da memoria o conteudo do endere?o do registrador
   MOV   R2, #GPIO_PORTK      ;LCD - PINOS DE DADOS
   ORR   R2, #GPIO_PORTM      ;LCD - PINOS DE COMANDO
   AND R1, R1, R2         ;seleciona apenas os pinos de porta de comparacao
   TST R1, R2            ;ANDS de R1 com R2
   BEQ Espera_porta      ;Se o flag Z=1, volta para o laco. Sen?o continua executando
   
   ; 2. Limpar o AMSEL para desabilitar a anal?gica
   LDR   R0, =GPIO_PORTK_AMSEL_R   ;LCD - PINOS DE DADOS
   LDR R1, [R0]
   BIC R1, #0xFF   ; Pinos PK0 a PK7 = 0: desabilita analogica
   STR R1, [R0]
   
   LDR   R0, =GPIO_PORTM_AMSEL_R   ;LCD - PINOS DE COMANDOS
   LDR R1, [R0]
   BIC R1, #2_111   ; Pinos M0 a M2 = 0: desabilita analogica
   STR   R1, [R0]
   
   ; 3. Limpar PCTL para selecionar o GPIO
   LDR   R0, =GPIO_PORTK_PCTL_R   ;LCD - PINOS DE DADOS
   LDR R1, [R0]
   BIC R1, #0xFF   ; Pinos PK0 a PK7 = 0: seleciona modo GPIO
   STR   R1, [R0]
   
   LDR   R0, =GPIO_PORTM_PCTL_R   ;LCD - PINOS DE COMANDOS
   LDR R1, [R0]
   BIC R1, #2_111   ; Pinos M0 a M2 = 0: seleciona modo GPIO
   STR   R1, [R0]

   ; 4. DIR para 0: input (BIC), 1: output (ORR)
   LDR   R0, =GPIO_PORTK_DIR_R
   LDR R1, [R0]
   ORR   R1, R1, #0xFF ; pinos PK0 a PK7 = 1: output
   STR   R1, [R0]
   
   LDR   R0, =GPIO_PORTM_DIR_R
   LDR R1, [R0]
   ORR R1, R1, #2_111 ; pinos M0 a M2 = 1: output
   STR R1, [R0]

   ; 5. Limpar os bits AFSEL para 0 para selecionar GPIO sem funcao alternativa
   LDR   R0, =GPIO_PORTK_AFSEL_R
   LDR R1, [R0]
   BIC R1, R1, #0xFF ; pinos PK0 a PK7 = 0: sem funcao alternativa
   STR   R1, [R0]
   
   LDR   R0, =GPIO_PORTM_AFSEL_R
   LDR R1, [R0]
   BIC R1, R1, #2_111 ;pinos M0 a M2 = 0: sem funcao alternativa
   STR   R1, [R0]
   
   ; 6. Setar os bits de DEN para habilitar I/O digital
   LDR   R0, =GPIO_PORTK_DEN_R   ;LCD - PINOS DE DADOS
   LDR R1, [R0]
   ORR   R1, R1, #0xFF ; pinos K0 a K7 = 1: habilita I/O digital
   STR   R1, [R0]
   
   LDR   R0, =GPIO_PORTM_DEN_R   ;LCD - PINOS DE COMANDOS
   LDR R1, [R0]
   ORR R1, R1, #2_111 ; pinos M0 a M3 = 1 : habilita I/O digital
   STR   R1, [R0]
   
   ; 7. Para habilitar resistor de pull-up interno, setar PUR para 1
   ; N/A

   BX   LR


; -------------------------------------------------------------------------------
; Funcao lcd_enable_and_wait - Da um enable no LCD, espera por 2ms e da um disable e espera 2ms
; Parametro de entrada: Nao tem
; Parametro de saida: Nao tem
lcd_enable_and_wait ;Depois dividir a funcao em wait 40us e wait 1,64ms
   ;EN como 1 para habilitar - EN -> PM2
   LDR R1, =GPIO_PORTM_DATA_R ;Carrega-se o endereco
   LDR R0, [R1] ; Le para carregar o valor anterior da porta inteira
   ORR R0, R0, #2_00000100 ; Faz o OR bit a bit para manter os valores anteriores e setar somente o bit
   STR R0, [R1] ; Escreve o novo valor da porta
   
   PUSH { LR }
   MOV R0, #10
   BL SysTick_Wait1us
   POP { LR }
   
   ;EN como 0 para desabilitar - EN -> PM2
   LDR R1, =GPIO_PORTM_DATA_R ;Carrega-se o endereco
   LDR R0, [R1] ; Le para carregar o valor anterior da porta inteira
   BIC R0, R0, #2_00000100 ; Faz o AND negado bit a bit para manter os valores anteriores e limpar somente o bit 0
   STR R0, [R1] ; Escreve o novo valor da porta
   
   PUSH { LR }
   MOV R0, #2
   BL SysTick_Wait1ms
   POP { LR }
   
   BX LR

; -------------------------------------------------------------------------------
; Funcao set_RS_0 - Seta o RS do LCD como 0
; Parametro de entrada: Nao tem
; Parametro de saida: Nao tem
set_RS_0
   ;RS como 0 para enviar instrucao - RS -> PM0   
   LDR R1, =GPIO_PORTM_DATA_R ;Carrega-se o endereco
   LDR R0, [R1] ; Le para carregar o valor anterior da porta inteira
   BIC R0, R0, #2_00000001 ; Faz o AND negado bit a bit para manter os valores anteriores e limpar somente o bit 0
   STR R0, [R1] ; Escreve o novo valor da porta
   
   BX LR

; -------------------------------------------------------------------------------
; Funcao set_RS_1 - Seta o RS do LCD como 1
; Parametro de entrada: Nao tem
; Parametro de saida: Nao tem
set_RS_1
   ;RS como 1 para enviar dados - RS -> PM0   
   LDR R1, =GPIO_PORTM_DATA_R ;Carrega-se o endereco
   LDR R0, [R1] ; Le para carregar o valor anterior da porta inteira
   ORR R0, R0, #2_00000001 ; Faz o OR bit a bit para manter os valores anteriores e setar somente o bit
   STR R0, [R1] ; Escreve o novo valor da porta
   
   BX LR

; -------------------------------------------------------------------------------
; Funcao envia_instrucao_lcd - Envia um comando para o LCD
; Parametro de entrada: R0 -> Comando a ser enviado
; Parametro de saida: Nao tem 
envia_instrucao_lcd
   LDR R1, =GPIO_PORTK_DATA_R
   STR R0, [R1]
   
   PUSH { LR }
   BL set_RS_0
   POP { LR }
   
   PUSH { LR }
   BL lcd_enable_and_wait
   POP { LR }
   
   BX LR

; -------------------------------------------------------------------------------
; Funcao envia_dado_lcd - Envia um dado para o LCD
; Parametro de entrada: R1 -> Dado a ser enviado
; Parametro de saida: Nao tem 
envia_dado_lcd
   LDR R0, =GPIO_PORTK_DATA_R
   STR R1, [R0]
   
   PUSH { LR }
   BL set_RS_1
   POP { LR }
   
   PUSH { LR }
   BL lcd_enable_and_wait
   POP { LR }
   
   BX LR

; -------------------------------------------------------------------------------
; Funcao LCD_printArrayInLcd - Escreve uma string no LCD
; Parametro de entrada: R0 -> Endereco de memoria de inicio da string
;                       R1 -> Tamanho da string
; Parametro de saida: Nao tem 
LCD_printArrayInLcd
   MOV R2, R1
escrever_proximo
   LDRB R1, [R0], #1
   PUSH { LR, R0 }
   BL envia_dado_lcd
   POP { R0, LR }
   SUB R2, #1
   CMP R2, #0
   BEQ LCD_printArrayInLcdEnd
   B escrever_proximo
LCD_printArrayInLcdEnd
   BX LR

; -------------------------------------------------------------------------------
; Funcao LCD_ResetLCD - Reseta o LCD
; Parametro de entrada: Nao tem
; Parametro de saida: Nao tem 
LCD_ResetLCD
   ; Reset LCD
   MOV R0, #0x01
   PUSH { LR, R1 }
   BL envia_instrucao_lcd
   POP { R1, LR }
   BX LR
   
; -------------------------------------------------------------------------------
; Funcao LCD_PulaCursorSegundaLinha - Manda o cursor para a segunda linha do LCD
; Parametro de entrada: Nao tem
; Parametro de saida: Nao tem 
LCD_PulaCursorSegundaLinha
   ; Reset LCD
   MOV R0, #0xC0
   PUSH { LR, R1 }
   BL envia_instrucao_lcd
   POP { R1, LR }
   BX LR

; -------------------------------------------------------------------------------
; LCD_SetCursorPos - Set cursor to desired position
; Input: R0 -> Cursor position (0x80 - 0x8F) first line or (0xC0 - 0xCF) second line
; Output: None
LCD_SetCursorPos
   PUSH { LR }
   BL envia_instrucao_lcd
   POP { LR }
LCD_SetCursorPosEnd
   BX LR

; -------------------------------------------------------------------------------
; Funcao LCD_init - Inicializa o LCD
; Parametro de entrada: Nao tem
; Parametro de saida: Nao tem 
LCD_init
   ; Inicializa configuracao do LCD 
   MOV R0, #0x38
   PUSH { LR }
   BL envia_instrucao_lcd
   POP { LR }
   
   ; Autoincrement to the right
   MOV R0, #0x6
   PUSH { LR }
   BL envia_instrucao_lcd
   POP { LR }

   ; Inicializa configuracao do LCD 
   MOV R0, #0xF
   PUSH { LR }
   BL envia_instrucao_lcd
   POP { LR }

   ; Reset no LCD 
   PUSH { LR }
   BL LCD_ResetLCD
   POP { LR }

   BX LR

; -------------------------------------------------------------------------------
; fim do arquivo
   ALIGN                           ; garante que o fim da se??o est? alinhada 
    END                             ; fim do arquivo