#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "Timer2.h"

void togglePB2(void){
	GPIO_PORTB_DATA_R ^= 0x04;	// Toggle PB2
}

// Initialize PB2 as a digital output for the speaker
void portBInit(void){
	SYSCTL_RCGCGPIO_R |= 0x02;            // activate port b
	GPIO_PORTB_DIR_R |= 0x04;							// PB2 output
	GPIO_PORTB_AFSEL_R &= ~0x04;					// Disable alternate function
	GPIO_PORTB_DEN_R |= 0x04;							// Enable digital I/0
}

// Initialize the speaker - the period will determine the tone that is played
void Speaker_Init(unsigned long period){
		portBInit();
		Timer2_Init(&togglePB2, period);
}

void StartAlarm(void){
	TIMER2_CTL_R = 0x00000001;    // 10) enable timer2A
}

void StopAlarm(void){
	TIMER2_CTL_R = 0x00000000;    // 10) enable timer2A
}
