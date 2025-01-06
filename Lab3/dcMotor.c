
#include "tm4c1294ncpdt.h"
#include <stdint.h>

#define GPIO_PORTH 0x80 //bit 7

void SysTick_Wait1ms(uint32_t delay);

// Full-step sequence (4 steps)
int fullStepSeq[4] = {0xE, 0xD, 0xB, 0x7};

// Half-step sequence (8 steps)
int halfStepSeq[8] = {0x1, 0x3, 0x2, 0x6, 0x4, 0xC, 0x8, 0x9};

typedef enum sentido{
   CLOCKWISE,
   COUNTER_CLOCKWISE,
} Sentido;

typedef enum bool
{
   false,
   true
} bool;

extern void PortF_Output(uint32_t valor);

extern void dcMotor_init(void){
   // clk da porta
   SYSCTL_RCGCGPIO_R |= GPIO_PORTH;

   // espera porta estar pronta
   while((SYSCTL_PRGPIO_R & GPIO_PORTH) != GPIO_PORTH){};
   
   // 2. Limpar o AMSEL para desabilitar a analogica
   GPIO_PORTH_AHB_AMSEL_R |= 0x00;

   // 3. Limpar PCTL para selecionar o GPIO
   GPIO_PORTH_AHB_PCTL_R |= 0x00;

   // 4. DIR para 0 se for entrada, 1 se for saida
   GPIO_PORTH_AHB_DIR_R |= 0x0F;

   // 5. Limpar os bits AFSEL para 0 para selecionar GPIO sem funcao alternativa
   GPIO_PORTH_AHB_AFSEL_R |= 0x00;

   // 6. Setar os bits de DEN para habilitar I/O digital
   GPIO_PORTH_AHB_DEN_R |= 0x0F;

   return;
}

extern void PortH_Output(unsigned long data){
   unsigned long temp;
   temp = GPIO_PORTH_AHB_DATA_R & 0xFFFFFFF0;
   temp = temp | data;
   GPIO_PORTH_AHB_DATA_R = temp;
   return;
}

extern void stepMotorControl(int steps, char direction, bool fullStepMode) {
    int sequenceLength = 0;
    int *stepSequence;

    if (fullStepMode) {
        stepSequence = fullStepSeq;  
        sequenceLength = 4;
    } else {
        stepSequence = halfStepSeq;
        sequenceLength = 8;
    }

    for (int i = 0; i < steps; i++) {
        int stepIndex;
        if (direction == 'C') {
            stepIndex = (sequenceLength - i - 1) % sequenceLength;
        } else {
            stepIndex = (sequenceLength - i - 1) % sequenceLength;
        }

        PortH_Output(stepSequence[stepIndex]);
        SysTick_Wait1ms(5);
    }
}

extern void dcMotor_rotateMotor(Sentido enMotorDirection, bool fullStep, int increment) {
    if (fullStep) {
        if (enMotorDirection == CLOCKWISE) {
            stepMotorControl(increment, 'C', true); 
        } else {
            stepMotorControl(increment, 'X', true); 
        }
    } else {
        if (enMotorDirection == CLOCKWISE) {
            stepMotorControl(increment, 'C', false); 
        } else {
            stepMotorControl(increment, 'X', false); 
        }  
    }
}


