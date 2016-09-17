/*
Module to keep the current time
*/

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "Timer1.h"
#include "Heartbeat.h"
#include "Speaker.h"

// Masks to get the hour or minute part of the encoded time
#define MINUTE_MASK 0x3F
#define HOUR_MASK 0x7C0

static int currentTime;
static int currentAlarm;
static int currentSeconds;	// Not part of the encoded time.  Used only by this file to determine when to increment the minutes

static int alarmHit;	// 0 - alarm off, 1 - alarm on

//Input: time format Ouput: minute
int Get_Minutes(){
	return currentTime & MINUTE_MASK;
}

//Input: time format Output: hours
int Get_Hours(){
	return (currentTime & HOUR_MASK) >> 6;
}

int Get_Alarm_Minutes(){
	return currentAlarm & MINUTE_MASK;
}

int Get_Alarm_Hours(){
	return (currentAlarm & HOUR_MASK) >> 6;
}

void Set_Time(int h, int m){
	currentTime = ((h << 6) & HOUR_MASK) + m;
}

void Set_Alarm(int h, int m){
	currentAlarm = ((h << 6) & HOUR_MASK) + m;
}

//Input: hour, minute Output: time format
void formatTime(int h, int m){
	currentTime = (h << 6) + m;
}

// Start the alarm ISR if the alarm equals the time
void Check_Alarm(){
	if(alarmHit){
		StartAlarm();
	}
}


// Increment the time by 1 second
void incrementTime(){
	int minutes = Get_Minutes();
	int hours = Get_Hours();
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
	
	if(currentTime == currentAlarm) alarmHit = 1;	// Check the alarm - set value if hit
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
	currentAlarm = 0x03;
	Timer1_Init(&Timer1_Handler, 79999999);
}
