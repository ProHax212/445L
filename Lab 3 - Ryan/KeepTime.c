/*
Module to keep the current time
*/

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "Timer1.h"

#define MINUTE_MASK 0x3F
#define HOUR_MASK 0x7C0

static int currentTime;

// Handler for the timer - Increment the time by 1 minute
void Timer1_Handler(void){
	
	int minutes = currentTime & MINUTE_MASK;
	int hours = (currentTime & HOUR_MASK) >> 6;
	
	minutes += 1;
	
	// Check if hour should change
	if(minutes >= 60){
		minutes = 0;
		hours += 1;
	}
	
	// Check if day resets
	if(hours >= 24){
			minutes = 0;
			hours = 0;
	}
	
	// Update the currentTime variable
	currentTime = (hours << 6) + minutes;
	
}

// Initialize the timer to start counting time
// Assumption: The bus clock speed is 80MHz
void KeepTime_Init(void){
	currentTime = 59 + (23 << 6);
	Timer1_Init(&Timer1_Handler, 80000000);
}
