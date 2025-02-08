// Authors: Caio Andrade and Joao Caversan

#include <stdint.h>
#include "tm4c1294ncpdt.h"

#define GPIO_PORTJ  (0x0100) //bit 8
#define GPIO_PORTN  (0x1000) //bit 12
#define GPIO_PORTF  (0x0020) // bit 5

#define INVALID_NUMBER 0xFF

///////// EXTERNAL FUNCTIONS INCLUSIONS //////////
// Since there is no .h in most files, theis functions must be included by hand.
// Same as if we were using IMPORT from assembly

void SysTick_Wait1ms(uint32_t delay);

/////// EXTERNABLE FUNCTIONS DECLARATION ///////

extern void uart_uartInit(void);
extern unsigned char uart_uartRx(void);
extern unsigned char uart_uartRxToInt(void);
extern void uart_uartTx(unsigned char txMsg);
extern void uart_uartTxString(char *pcString, unsigned char ucStrSize);
extern void uart_uartTxIntToChar(unsigned char ucNumber);
extern void uart_clearTerminal(void);

/////// EXTERNABLE FUNCTIONS IMPLEMENTATION ///////

/**
 * @brief Initializes all UART registers
 */
extern void uart_uartInit(void)
{
   // UART SETTINGS
   SYSCTL_RCGCUART_R = SYSCTL_RCGCUART_R0; // Enables clk

   while ((SYSCTL_PRUART_R & SYSCTL_PRUART_R0) != SYSCTL_PRUART_R0) { } // Waits for clock to be ready

   UART0_CTL_R = UART0_CTL_R & (~UART_CTL_UARTEN); // Disables uart0 by setting uarten to 0

   UART0_IBRD_R = 260; // Magic numbers from slide show -> sysclock/(clkDiv * BaudRate) = 80M/(16*19200)
   UART0_FBRD_R = 27; // round(Decimal number * 64)

   UART0_LCRH_R = 0x7A; // WLEN = 11, FEN = 1, STP2 = 1, EPS = 0, PEN = 1

   UART0_CC_R = 0; // CLK = sysCLK

   UART0_CTL_R = (UART_CTL_UARTEN | UART_CTL_TXE | UART_CTL_RXE); // Enables Tx, Rx, HSE=0 (clkDiv = 16) and UARTEN

   SysTick_Wait1ms(30);

   return;
}

/**
 * @brief Receives data from UART0 if the queue is not empty
 * 
 * @return unsigned char The byte received. 0 if nothing was received
 */
extern unsigned char uart_uartRx(void)
{
   unsigned char rxMsg = 0;
   unsigned long isRxQueueEmpty = (UART0_FR_R & UART_FR_RXFE) >> 4;

   if (0 == isRxQueueEmpty)
   {
      rxMsg = UART0_DR_R;
   }

   return rxMsg;
}

/**
 * @brief Gets a number from the UART0 and converts it to an integer
 * 
 * If no number is received, INVALID_NUMBER is returned
 * 
 * @return unsigned char The number from 0 to 9 received. INVALID_NUMBER if no number was received
 */
extern unsigned char uart_uartRxToInt(void)
{
   unsigned char rxMsg = uart_uartRx();

   if (rxMsg >= '0' && rxMsg <= '9')
   {
      rxMsg -= '0';
   }
   else
   {
      rxMsg = INVALID_NUMBER;
   }

   return rxMsg;
}

/**
 * @brief Transmits data to UART0 if the queue is not full and txMsg differs from 0
 * 
 * @param txMsg The byte to be sent
 */
extern void uart_uartTx(unsigned char txMsg)
{
   while((UART0_FR_R & UART_FR_TXFF) >> 5) { } // Waits for Tx queue to be not full

   if (0 != txMsg)
   {
      UART0_DR_R = txMsg;
   }

   return;
}

/**
 * @brief Transmits a string to UART0
 * 
 * @param pcString  The string to be sent
 * @param ucStrSize The size of the string
 */
extern void uart_uartTxString(char *pcString, unsigned char ucStrSize)
{
   unsigned char ucIndex = 0;

   for (ucIndex = 0; ucIndex < ucStrSize; ucIndex++)
   {
      if ('\0' == pcString[ucIndex])
      {
         break;
      }

      uart_uartTx(pcString[ucIndex]);
   }

   return;
}

/**
 * @brief Transmits a number from 0 to 9 to UART0 as it's corresponding char
 * 
 * @param ucNumber The number to be sent
 */
extern void uart_uartTxIntToChar(unsigned char ucNumber)
{
   unsigned char ucChar = 0;

   if (ucNumber <= 9)
   {
      ucChar = ucNumber + '0';
      uart_uartTx(ucChar);
   }

   return;
}

/**
 * @brief Clears the terminal
 */
extern void uart_clearTerminal(void)
{
   // "\033[2J" clears screen
   // "\033[H" takes cursor to the top left corner
   
   uart_uartTxString("\033[2J\033[H", 7);
}
