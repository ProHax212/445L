/*
Module to keep the current time
*/

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "Timer1.h"
#include "Heartbeat.h"
#include "Speaker.h"
#include "LCDInterface.h"
#include "Switch.h"
#include "AlarmClockTestMain.h"

// Masks to get the hour or minute part of the encoded time
#define MINUTE_MASK 0x3F
#define HOUR_MASK 0x7C0

static int currentTime;
static int currentAlarm;
static int secondsCounter;	// Not part of the encoded time.  Used only by this file to determine when to increment the minutes

static int alarmHit;	// 0 - alarm off, 1 - alarm on


// Output: time format
int Get_Time(){
	return currentTime;
}

//Output: time format
int Get_Alarm(){
	return currentAlarm;
}

//Input: time format
//Output: minutes
int Get_Minutes(int time){
	return time & MINUTE_MASK;
}

//Input: time format
//Output: hours
int Get_Hours(int time){
	return (time & HOUR_MASK) >> 6;
}

//Input: hours, minutes
//Ouput: time format
int Get_Time_Format(int hours, int minutes){
	return (hours << 6) + minutes;
}

//Input: time format
void Set_Time(int time){
	currentTime = time;
}

//Input: time format
void Set_Alarm(int alarm){
	currentAlarm = alarm;
}

void Format_Time(int h, int m){
	currentTime = Get_Time_Format(h, m);
}

void Format_Alarm(int h, int m){
	currentAlarm = Get_Time_Format(h, m);
}

// Start the alarm ISR if the alarm equals the time
void Check_Alarm(){
	if(alarmHit){
		Start_Alarm();
	}
}

// Input: time, seconds
// Output: time + seconds
int incrementTime(int t, int s){
	int minutes = Get_Minutes(t);
	int hours = Get_Hours(t);
	
	if(s >= 60){//Increment minutes
		minutes += s/60;
	}
	if(minutes >= 60){//Check for hour reset
		hours += minutes/60;
		minutes %= 60;
	}
	if(hours >= 24) hours %= 24;	//Check for day reset
	
	return Get_Time_Format(hours, minutes);
}

// Handler for the timer - Increment the time each second
void Timer1_Handler(void){
	Toggle_Heartbeat();
	secondsCounter += 60;//60 for debugging to make 1s = 1m
	if(secondsCounter >= 60){
		currentTime = incrementTime(currentTime, secondsCounter);//Increment time by seconds
		Clock_Tick(currentTime, currentAlarm);
		secondsCounter %= 60;
	}
	if(currentTime == currentAlarm) alarmHit = 1;	// Check the alarm - set value if hit
}

// Initialize the timer to start counting time
// Assumption: The bus clock speed is 80MHz (for the period of Timer 1)
void KeepTime_Init(int timeHours, int timeMinutes, int alarmHours, int alarmMinutes){
	secondsCounter = 0;
	Format_Time(timeHours, timeMinutes);
	Format_Alarm(alarmHours, alarmMinutes);
	Timer1_Init(&Timer1_Handler, 79999999);
}
