#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"

// ADCX initialization function(uses ADC0)
// measures voltages  from PE4, channel 9
void ADC_Init(void){
	volatile uint32_t delay;
	SYSCTL_RCGCADC_R = 0x01; //activate ADC0
  SYSCTL_RCGCGPIO_R |= 0x10;      // 1) activate clock for Port E
  while((SYSCTL_PRGPIO_R&0x10) == 0){};
	delay = SYSCTL_RCGCADC_R;       // extra time to stabilize
  delay = SYSCTL_RCGCADC_R;       // extra time to stabilize
  delay = SYSCTL_RCGCADC_R;       // extra time to stabilize
  delay = SYSCTL_RCGCADC_R;
  GPIO_PORTE_DIR_R &= ~0x30;      // 2) make PE4,PE5 input
  GPIO_PORTE_AFSEL_R |= 0x30;     // 3) enable alternate fun(analog, not dig functionality) on PE4,PE5
  GPIO_PORTE_DEN_R &= ~0x30;      // 4) disable digital I/O on PE4,PE5(analog voltages from 0 to 3.3V are coming in)
  GPIO_PORTE_AMSEL_R |= 0x30;     // 5) enable analog fun on PE4,PE5
  ADC0_PC_R = 0x1;               // 7) configure for 125K samples/sec 
  ADC0_SSPRI_R = 0x3210;          // 8) Seq 3 is lowest priority
  ADC0_ACTSS_R &= ~0x0004;        // 9) disable sample sequencer 3
  ADC0_EMUX_R &= ~0xF000;         // 10) seq3 is software trigger
	ADC0_SSMUX2_R = 0x0089;  // 11) 2 channels: Ain9 (PE4), Ain8(PE5) 
  ADC0_SSCTL2_R = 0x0060;         // 12) no TS0 D0, yes IE0 END0
  ADC0_IM_R &= ~0x0004;           // 13) disable SS2 interrupts
  ADC0_ACTSS_R |= 0x0004;         // 14) enable sample sequencer 2
}

//------------ADC_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
// measures from PE4, analog channel 9
void ADC_In89(uint16_t data[2]){  
	//uint32_t Data;
  ADC0_PSSI_R = 0x0004;            // 1) initiate SS2(sequencer 2)... start ADC conversion
  while((ADC0_RIS_R&0x04)==0){};   // 2) wait for conversion done
  data[1] = ADC0_SSFIFO2_R&0xFFF;   // 3) read result
	data[0] = ADC0_SSFIFO2_R&0xFFF;   // 3) read result
  ADC0_ISC_R = 0x0004;             // 4) acknowledge completion so RIS flag reset back to 0;
  // return Data;
}