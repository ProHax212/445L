/*
Module for the heartbeat of the system
*/

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"

// Initialize PF1 to be the heartbeat LED
void Heartbeat_Init(void){
	SYSCTL_RCGCGPIO_R |= 0x20;            // activate port F
  GPIO_PORTF_DIR_R |= 0x02;             // make PF1 out (built-in LED)
  GPIO_PORTF_AFSEL_R &= ~0x02;          // disable alt funct on PF1
  GPIO_PORTF_DEN_R |= 0x02;             // enable digital I/O on PF1
  GPIO_PORTF_AMSEL_R = 0;               // disable analog functionality on PF
  GPIO_PORTF_DATA_R &= ~0x02;           // turn off LED
}

void Toggle_Heartbeat(void){
	GPIO_PORTF_DATA_R ^= 0x02;
	
	int i = 800000;
	while(i > 0) i -= 1;	// Wait 10 ms
	
	GPIO_PORTF_DATA_R ^= 0x02;
}
