// Header file for the adc module of Lab 4

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
void ADC0_InitSWTriggerSeq3_Ch9(void);

//------------ADC0_InSeq3------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
void ADC0_InSeq3(char printResult[15]);

//------------ADC0_InSeq2-------------
// Output: Internal temp
void ADC0_InSeq2(char printResult[15]);

void ADC0_InSeq3Number(char printResult[15]);
