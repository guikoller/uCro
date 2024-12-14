
#include "tm4c1294ncpdt.h"
#include <stdint.h>

#define GPIO_PORTE 0x10 //bit 7

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
   SYSCTL_RCGCGPIO_R |= GPIO_PORTE;

   // espera porta estar pronta
   while((SYSCTL_PRGPIO_R & GPIO_PORTE) != GPIO_PORTE){};
   
   // 2. Limpar o AMSEL para desabilitar a analogica
   GPIO_PORTE_AHB_AMSEL_R |= 0x00;

   // 3. Limpar PCTL para selecionar o GPIO
   GPIO_PORTE_AHB_PCTL_R |= 0x00;

   // 4. DIR para 0 se for entrada, 1 se for saida
   GPIO_PORTE_AHB_DIR_R |= 0x0C;

   // 5. Limpar os bits AFSEL para 0 para selecionar GPIO sem funcao alternativa
   GPIO_PORTE_AHB_AFSEL_R |= 0x00;

   // 6. Setar os bits de DEN para habilitar I/O digital
   GPIO_PORTE_AHB_DEN_R |= 0x0C;

   PortF_Output(0x0000000C); // Enables all H-Bridge pins - F2 and F3

   return;
}

extern void PortE_Output(unsigned long data){
   unsigned long temp;
   temp = GPIO_PORTE_AHB_DATA_R & 0xFFFFFFF0;
   temp = temp | data;
   GPIO_PORTE_AHB_DATA_R = temp;
   return;
}

extern void dcMotor_rotateMotor(Sentido enMotorDirection, bool bRotate)
{
   if (true == bRotate)
   {
      if (CLOCKWISE == enMotorDirection)
      {
         PortE_Output(0x00000004);
      }
      else
      {
         PortE_Output(0x00000008);
      }
   }
   else
   {
      PortE_Output(0x00000000);
   }

   return;
}