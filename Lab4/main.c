#include <stdint.h>
#include <stdio.h>  // For snprintf
#include <string.h> // For strlen
#include "tm4c1294ncpdt.h"

///////// DEFINES AND MACROS //////////
#define TICKS_FOR_1MS 80000

typedef enum {
    INIT,
    WAIT_START,
    CHOOSE_CONTROL_MODE,
    TERMINAL_CONTROL,
    POTENTIOMETER_CONTROL,
    STOP_MOTOR
} State;

typedef enum bool{
   false,
   true
} bool;


///////// EXTERNAL FUNCTIONS INCLUSIONS //////////

void PLL_Init(void);
void SysTick_Init(void);
void SysTick_Wait1ms(uint32_t delay);
void SysTick_Wait1us(uint32_t delay);     

void GPIO_Init(void);
uint32_t PortJ_Input(void);
void PortN_Output(uint32_t leds);
void PortF_Output(uint32_t valor);

extern void timerInit(void);
extern void motor_init(void);
extern void rotate_motor(bool clockwise, bool rotate);
extern void adc_adcInit(void);
extern void adc_startAdcConversion(void);
extern void uart_uartInit(void);

///////// LOCAL FUNCTIONS DECLARATIONS //////////
static void init(void);
static void handle_states(void);

///////// STATIC VARIABLES DECLARATIONS //////////
static State currentState = INIT;

int main(void)
{
    while (1){
        handle_states();
    }
}

void handle_states(void){
    switch (currentState){
        case INIT:
            init();
            currentState = WAIT_START;
            break;
        case WAIT_START:
            break;
        case CHOOSE_CONTROL_MODE:
            break;
        case TERMINAL_CONTROL:
            break;
        case POTENTIOMETER_CONTROL:
            break;
        case STOP_MOTOR:
            break;
        default:
            break;
    }
}

void init(void){
    PLL_Init();
    SysTick_Init();
    GPIO_Init();
    timerInit();
		motor_init();
    adc_adcInit();
    uart_uartInit();
    return;
}


void Timer2A_Handler(void)
{
    TIMER2_ICR_R = 1; // ACKS the interruption
}
