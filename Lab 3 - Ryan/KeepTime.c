/*
Module to keep the current time
*/

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "Timer1.h"
#include "Heartbeat.h"

// Masks to get the hour or minute part of the encoded time
#define MINUTE_MASK 0x3F
#define HOUR_MASK 0x7C0

static int currentTime;
static int currentAlarm;

static int currentSeconds;	// Not part of the encoded time.  Used only by this file to determine when to increment the minutes

//Input: time format Ouput: minute
int getMinutes(){
	return currentTime & MINUTE_MASK;
}

//Input: time format Output: hours
int getHours(){
	return (currentTime & HOUR_MASK) >> 6;
}

//Input: hour, minute Output: time format
void formatTime(int h, int m){
	currentTime = (h << 6) + m;
}

// Increment the time by 1 second
void incrementTime(){
	int minutes = getMinutes();
	int hours = getHours();
	//currentSeconds += 1;
	 currentSeconds += 60;	// Use this for debugging - makes each second a minute
	
	// Check to increment minutes
	if(currentSeconds >= 60){
		minutes += 1;
		currentSeconds = 0;
	}
	if(minutes >= 60){//Check for hour reset
		hours += 1;
		minutes = 0;
	}
	if(hours >= 24) hours = 0;	//Check for day reset
	
	formatTime(hours, minutes);
}

// Handler for the timer - Increment the time by 1 second
void Timer1_Handler(void){
	Toggle_Heartbeat();
	incrementTime();
}

// Initialize the timer to start counting time
// Assumption: The bus clock speed is 80MHz (for the period of Timer 1)
void KeepTime_Init(void){
	currentSeconds = 0;
	formatTime(23, 59);
	currentAlarm = 0;
	Timer1_Init(&Timer1_Handler, 80000000);
}
