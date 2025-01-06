#include <stdint.h>
#include <stdio.h>  // For snprintf
#include <string.h> // For strlen
#include "tm4c1294ncpdt.h"

///////// DEFINES AND MACROS //////////

#define TICKS_FOR_1MS 80000

typedef enum sentido
{
   CLOCKWISE,
   COUNTER_CLOCKWISE,
} Sentido;

typedef enum bool
{
   false,
   true
} bool;

typedef enum
{
    STATE_INIT,
    STATE_WAIT_FOR_INPUT,
    STATE_ROTATE_MOTOR,
    STATE_DISPLAY_RESULT
} State;

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
extern void dcMotor_rotateMotor(Sentido enMotorDirection, bool bRotate, int increment);

void LCD_GPIOinit(void);
void LCD_init(void);
void LCD_printArrayInLcd(uint8_t *str, uint8_t size);
void LCD_ResetLCD(void);
void LCD_PulaCursorSegundaLinha(void);
void LCD_SetCursorPos(unsigned char ucPos);

void MKBOARD_GPIOinit(void);
unsigned char MKEYBOARD_readKeyboard(void);

///////// LOCAL FUNCTIONS DECLARATIONS //////////

static void initVars(void);
static void handleState(void);
static void displayIncrementAndDirection(char key);
static int	updatePositionAndRotations(char key);
static void displayPositionAndRotations(void);
static void displayStepMode(void);
static void rotateMotor(char key);
static void displayMotorSpeed(void);

///////// STATIC VARIABLES DECLARATIONS //////////

static State currentState = STATE_WAIT_FOR_INPUT;
static char lastKey = 0;
static int motorPosition = 0;
static int motorRotations = 0;
static bool fullStepMode = true;
static Sentido direction = CLOCKWISE; // Global direction variable

///////// LOCAL FUNCTIONS IMPLEMENTATIONS //////////

int main(void)
{
    PLL_Init();
    SysTick_Init();
    GPIO_Init();
    LCD_GPIOinit();
    LCD_init();
    MKBOARD_GPIOinit();
    timerInit();
    dcMotor_init();

    // Enable Port J interrupt
    
    GPIO_PORTJ_AHB_IM_R &= ~0x03;    
    GPIO_PORTJ_AHB_IS_R &= ~0x03;   
    GPIO_PORTJ_AHB_IBE_R &= ~0x03;  
    GPIO_PORTJ_AHB_IEV_R |= 0x03;  
		GPIO_PORTJ_AHB_ICR_R  |= 0x03;
		GPIO_PORTJ_AHB_IM_R |= 0x03; 
		NVIC_EN1_R |= (1 << (51 - 32));

    while (1)
    {
        handleState();
    }
}

static void initVars(void)
{
    LCD_ResetLCD();
    //SysTick_Wait1ms(1000);
    motorPosition = 0;
    motorRotations = 0;
    currentState = STATE_WAIT_FOR_INPUT;
    return;
}

static void handleState(void)
{
    switch (currentState)
    {
    case STATE_INIT:
        initVars();
        break;
    case STATE_WAIT_FOR_INPUT:
        lastKey = MKEYBOARD_readKeyboard();
				if(lastKey != 0)
					currentState = STATE_ROTATE_MOTOR;
				else
					currentState = STATE_WAIT_FOR_INPUT;
        break;
    case STATE_ROTATE_MOTOR:
        rotateMotor(lastKey);
        currentState = STATE_DISPLAY_RESULT;
        break;
    case STATE_DISPLAY_RESULT:
				LCD_ResetLCD();
        displayPositionAndRotations();
        SysTick_Wait1ms(300);
        currentState = STATE_WAIT_FOR_INPUT;
        break;
    default:
        currentState = STATE_WAIT_FOR_INPUT;
        break;
    }
}

static void rotateMotor(char key)
{
		int increment;
    increment = updatePositionAndRotations(key);
		if(key != 0){
			increment = increment * (2048/360);
			// Rotate the motor based on the global direction
			dcMotor_rotateMotor(direction, true, increment);
		}
}

static int updatePositionAndRotations(char key)
{
    int increment = 0;

    switch (key)
    {
    case '1':
        increment = 15;
        direction = CLOCKWISE;
        break;
    case '2':
        increment = 30;
        direction = CLOCKWISE;
        break;
    case '3':
        increment = 45;
        direction = CLOCKWISE;
        break;
    case '4':
        increment = 60;
        direction = CLOCKWISE;
        break;
    case '5':
        increment = 90;
        direction = CLOCKWISE;
        break;
    case '6':
        increment = 180;
        direction = CLOCKWISE;
        break;
    case '7':
        increment = 15;
        direction = COUNTER_CLOCKWISE;
        break;
    case '8':
        increment = 30;
        direction = COUNTER_CLOCKWISE;
        break;
    case '9':
        increment = 45;
        direction = COUNTER_CLOCKWISE;
        break;
    case 'A':
        increment = 60;
        direction = COUNTER_CLOCKWISE;
        break;
    case 'B':
        increment = 90;
        direction = COUNTER_CLOCKWISE;
        break;
    case 'C':
        increment = 180;
        direction = COUNTER_CLOCKWISE;
        break;
    default:
        return 0;
    }

    if (direction == CLOCKWISE)
    {
        motorPosition += increment;
        if (motorPosition >= 360)
        {
            motorPosition -= 360;
            motorRotations++;
        }
    }
    else
    {
        motorPosition -= increment;
        if (motorPosition < 0)
        {
            motorPosition += 360;
            motorRotations--;
        }
    }

    // Ensure motorRotations is updated correctly when crossing zero boundary
    if (motorPosition < 0)
    {
        motorPosition += 360;
        motorRotations--;
    }
    else if (motorPosition >= 360)
    {
        motorPosition -= 360;
        motorRotations++;
    }
		return increment;
}



//o que falta: printar a velocidade, o motor nao esta girando no sentido anti -horarioe precisa arruma a funcao do tempo(timer)

static void displayPositionAndRotations(void)
{
    char positionStr[16];
    snprintf(positionStr, sizeof(positionStr), "Pos: %d", motorPosition);
    LCD_SetCursorPos(0x80);
    LCD_printArrayInLcd((uint8_t *)positionStr, strlen(positionStr));

    char rotationStr[16];
    snprintf(rotationStr, sizeof(rotationStr), "Rot: %d", motorRotations);
    LCD_SetCursorPos(0xC0);
    LCD_printArrayInLcd((uint8_t *)rotationStr, strlen(rotationStr));
}

static void displayStepMode(void)
{
    char stepModeStr[16];
    if (fullStepMode)
    {
        snprintf(stepModeStr, sizeof(stepModeStr), "Step: Full");
    }
    else
    {
        snprintf(stepModeStr, sizeof(stepModeStr), "Step: Half");
    }
		LCD_ResetLCD();
    LCD_SetCursorPos(0x00);
    LCD_printArrayInLcd((uint8_t *)stepModeStr, strlen(stepModeStr));
		SysTick_Wait1ms(1000);
}

///////// HANDLERS IMPLEMENTATIONS //////////

void GPIOPortJ_Handler(void)
{
    if (GPIO_PORTJ_AHB_RIS_R & 0x01) // USR_SW1 (muda o modo de passo)
    {
        fullStepMode = !fullStepMode;
        displayStepMode();
			  GPIO_PORTJ_AHB_ICR_R = 0x01; // Clear interrupt
    }
    if (GPIO_PORTJ_AHB_RIS_R & 0x02) // USR_SW2
    {
        motorPosition = 0;
        motorRotations = 0;
        displayPositionAndRotations();
				GPIO_PORTJ_AHB_ICR_R = 0x02; // Clear interrupt
    }
}

void Timer2A_Handler(void)
{
    TIMER2_ICR_R = 1; // ACKS the interruption
    return;
}