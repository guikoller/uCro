#include <stdint.h>
#include "tm4c1294ncpdt.h"

#define INVALID_ADC_VALUE 0xF000

extern void adc_adcInit(void);
extern void adc_startAdcConversion(void);

/**
 * @brief Initializes the SS3 sequencer of ADC0 as an interruption
 */
extern void adc_adcInit(void)
{
   SYSCTL_RCGCADC_R = SYSCTL_RCGCADC_R0; // Enables clk

   // Waits for clock to be ready
   while ((SYSCTL_PRADC_R & SYSCTL_PRADC_R0) != SYSCTL_PRADC_R0) { } 

   ADC0_PC_R = 0x7; // Max conversion rate

   ADC0_SSPRI_R = 0x0; // No priority, since there is only one sequencer

   ADC0_ACTSS_R = 0x0; // ASEN3 = 0

   ADC0_EMUX_R = ADC_EMUX_EM3_PROCESSOR; // Processor trigger event -> ADC0_PSSI = 0x8 for SS3

   ADC0_SSMUX3_R = 0x00000009; // AIN9 (PE3) 

   ADC0_SSCTL3_R = ADC_SSCTL3_IE0 | ADC_SSCTL3_END0; // Interrupt enable and end of sequence

   ADC0_ACTSS_R |= ADC_ACTSS_ASEN3; // Enables sequencer 3

   return;
}

extern void adc_startAdcConversion(void)
{
   ADC0_PSSI_R = ADC_PSSI_SS3; // Starts conversion

   return;
}

extern void adc_stopAdc3Conversion(void)
{
   ADC0_PSSI_R &= ~ADC_PSSI_SS3; // Stops conversion of sequencer 3

   return;
}

/**
 * @brief Reads the ADC3 value, if the conversion is available
 * 
 * @return unsigned short INVALID_ADC_VALUE if the conversion is not ready, ADC0_SSFIFO3_R if it is
 */
extern unsigned short adc_readAdc3Value(void)
{
   unsigned short usAdcValue = INVALID_ADC_VALUE;

   if (ADC_RIS_INR3 == (ADC0_RIS_R & ADC_RIS_INR3)) // Checks if the conversion of SS3 is done
   {
      usAdcValue   = ADC0_SSFIFO3_R;   // Reads the value from FIFO
      ADC0_DCISC_R = ADC_DCISC_DCINT3; // ACKS the conversion
      ADC0_PSSI_R  = ADC_PSSI_SS3;     // Restarts conversion
   }

   return usAdcValue;
}
