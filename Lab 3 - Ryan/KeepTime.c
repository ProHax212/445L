/*
Module to keep the current time
*/

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "Timer1.h"

#define MINUTE_MASK 0x3F
#define HOUR_MASK 0x7C0

static int currentTime;
static int currentAlarm;

//Input: time format Ouput: minute
int getMinutes(int t){
	return t & MINUTE_MASK;
}

//Input: time format Output: hours
int getHours(int t){
	return (t & HOUR_MASK) >> 6;
}

//Input: hour, minute Output: time format
int formatTime(int h, int m){
	return (h << 6) + m;
}

//Checks that a time for overflow (no min > 59 or hour > 23)
int checkTime(int t){
	int minutes = getMinutes(t);
	int hours = getHours(t);
	if(minutes >= 60){//Check for hour reset
		hours += 1;
		minutes %= 60;
	}
	if(hours >= 24)//Check for day reset
		hours %= 24;
	return formatTime(hours, minutes);
}

//Adds 1 minute to either time or alarm
int incrementTime(int t){
	t += 1;
	return checkTime(t);
}

// Handler for the timer - Increment the time by 1 minute
void Timer1_Handler(void){
	currentTime = incrementTime(currentTime);
}

// Initialize the timer to start counting time
// Assumption: The bus clock speed is 80MHz
void KeepTime_Init(void){
	currentTime = formatTime(23, 59);
	currentAlarm = formatTime(0, 0);
	Timer1_Init(&Timer1_Handler, 80000000);
}
