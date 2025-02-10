// gpio.c
// Desenvolvido para a placa EK-TM4C1294XL
// Configura Ports para o uso nessa pr�tica
// Cont�m tamb�m fun��es que interagem com os GPIO

#include <stdint.h>
#include "tm4c1294ncpdt.h"

// Port  A     para a comunica��o UART
// Port  E     para o motor e potenci�metro
// Port  F     para o motor
#define GPIO_PORTA  (0x00000001)
#define GPIO_PORTE  (0x00000010)
#define GPIO_PORTF  (0x00000020)

void GPIO_Init(void);
void escrevePortE(uint32_t entrada);
void escrevePortF(uint32_t entrada);

// -------------------------------------------------------------------------------
// Fun��o GPIO_Init
// Par�metro de entrada: N�o tem
// Par�metro de sa�da: N�o tem
void GPIO_Init(void) {
	// 1. Ativar o clock para as portas
	uint32_t ports = (GPIO_PORTA | GPIO_PORTE | GPIO_PORTF);
	SYSCTL_RCGCGPIO_R = ports;
	while ( (SYSCTL_PRGPIO_R & ports) != ports ) {
		
	}
	
	// 2. AMSEL
	GPIO_PORTA_AHB_AMSEL_R = 0x00;
	GPIO_PORTE_AHB_AMSEL_R = 0x10;
	GPIO_PORTF_AHB_AMSEL_R = 0x00;
		
	// 3. PCTL
	GPIO_PORTA_AHB_PCTL_R = 0x11;
	GPIO_PORTE_AHB_PCTL_R = 0x00;
	GPIO_PORTF_AHB_PCTL_R = 0x00;

	// 4. DIR para 0 se for entrada, 1 se for sa�da
	GPIO_PORTE_AHB_DIR_R = 0x03; // bits 0 e 1 para o motor, bit  4     para o pot
	GPIO_PORTF_AHB_DIR_R = 0x04; // bit  2     para o motor

	// 5. AFSEL
	GPIO_PORTA_AHB_AFSEL_R = 0x03;
	GPIO_PORTE_AHB_AFSEL_R = 0x10;
	GPIO_PORTF_AHB_AFSEL_R = 0x00;

	// 6. DEN
	GPIO_PORTA_AHB_DEN_R = 0x03; // bits 0 e 1 para o UART
	GPIO_PORTE_AHB_DEN_R = 0x03; // bits 0 e 1 para o motor, bit  4     para o pot
	GPIO_PORTF_AHB_DIR_R = 0x04; // bit  2     para o motor

}

// -------------------------------------------------------------------------------
// Fun��o escrevePortE
// Escreve nos bits 0 e 1 do registrador DATA do PortE os bits 0 e 1 da vari�vel de entrada
// Par�metro de entrada: valores a serem escritos nos pinos 0 e 1
// Par�metro de sa�da: n�o tem
void escrevePortE(uint32_t entrada) {
	// ignora os outros bits de entrada
    uint32_t valor = entrada & 0x03;
	
    // escrita amig�vel
    GPIO_PORTE_AHB_DATA_R = (GPIO_PORTE_AHB_DATA_R & ~(0x03u)) | valor; 
}

// -------------------------------------------------------------------------------
// Fun��o escrevePortF
// Escreve no bit 2 do registrador DATA do PortF o bit 2 da vari�vel de entrada
// Par�metro de entrada: valor a ser escrito no pino 2
// Par�metro de sa�da: n�o tem
void escrevePortF(uint32_t entrada) {
	// ignora os outros bits de entrada
    uint32_t valor = entrada & 0x04;
	
    // escrita amig�vel
    GPIO_PORTF_AHB_DATA_R = (GPIO_PORTF_AHB_DATA_R & ~(0x04u)) | valor; 
}
