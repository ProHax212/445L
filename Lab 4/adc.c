#include "../inc/tm4c123gh6pm.h"
#include <stdint.h>
#include <stdio.h>

// ADC module for Lab 4
// Take an ADC sample and convert it to some measurement

// This initialization function sets up the ADC according to the
// following parameters.  Any parameters not explicitly listed
// below are not modified:
// Max sample rate: <=125,000 samples/second
// Sequencer 0 priority: 1st (highest)
// Sequencer 1 priority: 2nd
// Sequencer 2 priority: 3rd
// Sequencer 3 priority: 4th (lowest)
// SS3 triggering event: software trigger
// SS3 1st sample source: Ain9 (PE4)
// SS3 interrupts: enabled but not promoted to controller
void ADC0_InitSWTriggerSeq3_Ch9(void){ 
                                  // 1) activate clock for Port E
  SYSCTL_RCGCGPIO_R |= 0x10;
  while((SYSCTL_PRGPIO_R&0x10) != 0x10){};

	// PE1 Initialization
  GPIO_PORTE_DIR_R &= ~0x02;      // 2) make PE4 input
  GPIO_PORTE_AFSEL_R |= 0x02;     // 3) enable alternate function on PE1
  GPIO_PORTE_DEN_R &= ~0x02;      // 4) disable digital I/O on PE1
  GPIO_PORTE_AMSEL_R |= 0x02;     // 5) enable analog functionality on PE1
  SYSCTL_RCGCADC_R |= 0x0001;   // 7) activate ADC0
  while((SYSCTL_PRADC_R&0x0001) != 0x0001){};    // good code, but not yet implemented in simulator

	// SS3 Initialization - Voltage
  ADC0_PC_R &= ~0xF;              // 7) clear max sample rate field
  ADC0_PC_R |= 0x1;               //    configure for 125K samples/sec
  ADC0_SSPRI_R = 0x0123;          // 8) Sequencer 3 is highest priority
  ADC0_ACTSS_R &= ~0x0008;        // 9) disable sample sequencer 3
  ADC0_EMUX_R &= ~0xF000;         // 10) seq3 is software trigger
  ADC0_SSMUX3_R &= ~0x000F;       // 11) clear SS3 field
  ADC0_SSMUX3_R += 2;             //    set channel to PE1
  ADC0_SSCTL3_R = 0x0006;         // 12) no TS0 D0, yes IE0 END0
  ADC0_IM_R &= ~0x0008;           // 13) disable SS3 interrupts
  ADC0_ACTSS_R |= 0x0008;         // 14) enable sample sequencer 3
	ADC0_SAC_R = (ADC0_SAC_R & 0xFFFFFFF8) | ADC_SAC_AVG_64X;	// Sampling - 64x
		
	// SS2 Initialization - Internal Temp
  ADC0_ACTSS_R &= ~0x0004;        // 9) disable sample sequencer 2
  ADC0_EMUX_R &= ~0x0F00;         // 10) seq2 is software trigger
	ADC0_SSMUX2_R &= ~0x000F;       // 11) clear SS2 field
  ADC0_SSCTL2_R = 0x000A;         // 12) Internal temp on first sample, E0 yes
  ADC0_IM_R &= ~0x0004;           // 13) disable SS2 interrupts
  ADC0_ACTSS_R |= 0x0004;         // 14) enable sample sequencer 2
}

//------------ADC0_InSeq3------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: String representation of voltage
void ADC0_InSeq3(char printResult[15]){  
	uint32_t result;
  ADC0_PSSI_R = 0x0008;            // 1) initiate SS3
  while((ADC0_RIS_R&0x08)==0){};   // 2) wait for conversion done
    // if you have an A0-A3 revision number, you need to add an 8 usec wait here
  result = ADC0_SSFIFO3_R&0xFFF;   // 3) read result
		
	// Convert result to voltage (fixed point with scaling factor (1/100)
	result = result * ((3.3*100)/4095);
	sprintf(printResult, "Voltage~%1d.%2dV", result/100, result%100);
		
  ADC0_ISC_R = 0x0008;             // 4) acknowledge completion
}

// Same as ADC0_InSeq3, but doesn't format the string
void ADC0_InSeq3Number(char printResult[15]){
	uint32_t result;
  ADC0_PSSI_R = 0x0008;            // 1) initiate SS3
  while((ADC0_RIS_R&0x08)==0){};   // 2) wait for conversion done
    // if you have an A0-A3 revision number, you need to add an 8 usec wait here
  result = ADC0_SSFIFO3_R&0xFFF;   // 3) read result
		
	// Convert result to voltage (fixed point with scaling factor (1/100)
	result = result * ((3.3*100)/4095);
	sprintf(printResult, "%1d.%2d", result/100, result%100);
		
  ADC0_ISC_R = 0x0008;             // 4) acknowledge completion
}

//------------ADC0_InSeq2------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: String representation of internal temp
void ADC0_InSeq2(char printResult[15]){  
	uint32_t result;
  ADC0_PSSI_R = 0x0004;            // 1) initiate SS2
  while((ADC0_RIS_R&0x04)==0){};   // 2) wait for conversion done
    // if you have an A0-A3 revision number, you need to add an 8 usec wait here
  result = ADC0_SSFIFO2_R&0xFFF;   // 3) read result
		
	// Convert result to voltage (fixed point with scaling factor (1/100)
	result = result * ((3.3*100)/4095);
	sprintf(printResult, "Temp~%1d.%2dV", result/100, result%100);
		
  ADC0_ISC_R = 0x0004;             // 4) acknowledge completion
}
