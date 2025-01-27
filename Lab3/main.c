#include <stdint.h>
#include <stdio.h>  // For snprintf
#include <string.h> // For strlen
#include "tm4c1294ncpdt.h"

///////// DEFINES AND MACROS //////////
#define TICKS_FOR_1MS 80000


typedef enum{
    INIT,
    WAIT_INPUT,
    ROTATE_MOTOR,
    SHOW_ROTATIONS,
    CHANGE_STEP_MODE
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

void timerInit(void);

extern void dcMotor_init(void);
extern void rotateMotor(int increment, bool clockwise, bool fullStepMode);

void LCD_GPIOinit(void);
void LCD_init(void);
void LCD_printArrayInLcd(uint8_t *str, uint8_t size);
void LCD_ResetLCD(void);
void LCD_PulaCursorSegundaLinha(void);
void LCD_SetCursorPos(unsigned char ucPos);

void MKBOARD_GPIOinit(void);
unsigned char MKEYBOARD_readKeyboard(void);

///////// LOCAL FUNCTIONS DECLARATIONS //////////
static void init(void);
static void handle_states(void);
static void updatePositionAndRotations(unsigned char key);
static void enable_port_j_interruption(void);
static void show_rotations(void);
static void displayStepMode(void);
static void clear(void);

///////// STATIC VARIABLES DECLARATIONS //////////
static State currentState = INIT;
static char lastKey = 0;
static int motorPosition = 0;
static int motorRotations = 0;
static int increment = 0;
static bool clockwise = true;
static bool fullStepMode = true;

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
            currentState = WAIT_INPUT;
            break;
        case WAIT_INPUT:
            lastKey = MKEYBOARD_readKeyboard();
            if (lastKey != 0){
				updatePositionAndRotations(lastKey);
                currentState = ROTATE_MOTOR;
			}
            break;
        case ROTATE_MOTOR:
            rotateMotor(increment, clockwise, fullStepMode);
            currentState = SHOW_ROTATIONS;
            break;
        case SHOW_ROTATIONS:
            show_rotations();
            currentState = WAIT_INPUT;
            break;
        case CHANGE_STEP_MODE:
            if(fullStepMode){
                fullStepMode = false;
            }else{
                fullStepMode = true;
            }
						displayStepMode();
            currentState = SHOW_ROTATIONS;
            break;
    }
}

void init(void){
    PLL_Init();
    SysTick_Init();
    GPIO_Init();
    LCD_GPIOinit();
    LCD_init();
    MKBOARD_GPIOinit();
    timerInit();
    dcMotor_init();
    enable_port_j_interruption();
    
    LCD_ResetLCD();
    motorPosition = 0;
    motorRotations = 0;
    show_rotations();
}

void updatePositionAndRotations(unsigned char key){
    switch (key){
    case '1':
        increment = 15;
        clockwise = true;
        break;
    case '2':
        increment = 30;
        clockwise = true;
        break;
    case '3':
        increment = 45;
        clockwise = true;
        break;
    case '4':
        increment = 60;
        clockwise = true;
        break;
    case '5':
        increment = 90;
        clockwise = true;
        break;
    case '6':
        increment = 180;
        clockwise = true;
        break;
    case '7':
        increment = 15;
        clockwise = false;
        break;
    case '8':
        increment = 30;
        clockwise = false;
        break;
    case '9':
        increment = 45;
        clockwise = false;
        break;
    case 'A':
        increment = 60;
        clockwise = false;
        break;
    case 'B':
        increment = 90;
        clockwise = false;
        break;
    case 'C':
        increment = 180;
        clockwise = false;
        break;
    default:
        increment = 0;
    }

    if(clockwise){
        motorPosition += increment;
        if(motorPosition >= 360){
            motorPosition -= 360;
            motorRotations++;
        }
    }else{
        motorPosition -= increment;
        if(motorPosition < 0){
            motorPosition += 360;
            motorRotations--;
        }
    }
}

void show_rotations(void){
    LCD_ResetLCD();
    char positionStr[16];
    snprintf(positionStr, sizeof(positionStr), "Pos: %d", motorPosition);
    LCD_SetCursorPos(0x80);
    LCD_printArrayInLcd((uint8_t *)positionStr, strlen(positionStr));

    char rotationStr[16];
    snprintf(rotationStr, sizeof(rotationStr), "Rot: %d", motorRotations);
    LCD_SetCursorPos(0xC0);
    LCD_printArrayInLcd((uint8_t *)rotationStr, strlen(rotationStr));
}

void  displayStepMode(void){
    char stepModeStr[16];

    if (fullStepMode){
        snprintf(stepModeStr, sizeof(stepModeStr), "Step: Full");
    }else{
        snprintf(stepModeStr, sizeof(stepModeStr), "Step: Half");
    }

	LCD_ResetLCD();
    LCD_SetCursorPos(0x00);
    LCD_printArrayInLcd((uint8_t *)stepModeStr, strlen(stepModeStr));
    SysTick_Wait1ms(1000);
}

void enable_port_j_interruption(void){
    GPIO_PORTJ_AHB_IM_R &= ~0x03;    
    GPIO_PORTJ_AHB_IS_R &= ~0x03;   
    GPIO_PORTJ_AHB_IBE_R &= ~0x03;  
    GPIO_PORTJ_AHB_IEV_R |= 0x03;  
    GPIO_PORTJ_AHB_ICR_R  |= 0x03;
    GPIO_PORTJ_AHB_IM_R |= 0x03; 
    NVIC_EN1_R |= (1 << (51 - 32));
}

void GPIOPortJ_Handler(void)
{
    if (GPIO_PORTJ_AHB_RIS_R & 0x01) // USR_SW1 (muda o modo de passo)
    {
        currentState = CHANGE_STEP_MODE;
        GPIO_PORTJ_AHB_ICR_R = 0x01; // Clear interrupt
    }
    if (GPIO_PORTJ_AHB_RIS_R & 0x02) // USR_SW2
    {
        
				motorPosition = 0;
				motorRotations = 0;
				currentState = SHOW_ROTATIONS;
        GPIO_PORTJ_AHB_ICR_R = 0x02; // Clear interrupt
    }
}

void Timer2A_Handler(void)
{
    TIMER2_ICR_R = 1; // ACKS the interruption
}
