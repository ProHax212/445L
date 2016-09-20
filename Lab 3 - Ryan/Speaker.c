#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "Timer2.h"

typedef struct Alarm{
	int alarmSound[10];
	int alarmTime[10];	// Time to play the note (in milliseconds)
	int alarmLength;
} Alarm;

Alarm alarmOne = {.alarmSound = {305775, 272423, 242696}, .alarmTime = {500, 500, 500}, .alarmLength = 3};
Alarm alarmTwo = {.alarmSound = {200000, 152890}, .alarmTime = {1000, 1000}, .alarmLength = 2};
Alarm alarmThree = {.alarmSound = {305775, 242696, 204081, 152890, 204081, 242696}, .alarmTime = {500, 500, 500, 500, 500, 500}, .alarmLength = 6};
Alarm alarmFour = {.alarmSound = {305775, 242696, 305775, 204081, 305775, 152890}, .alarmTime = {500, 500, 500, 500, 500, 500}, .alarmLength = 6};
Alarm alarms[4];

int static currentAlarmType = 0;

int static alarmOn;

int static alarmCounterIndex;	// Which note in the alarm is currently being played
int static alarmCounter;	// Used to count up to the time that the note should play - look at alarmTime array
int static notePlaying = 0;	// If a note is currently being played
void togglePB2(void){
	if(!alarmOn) return;
	if(!notePlaying){
		notePlaying = 1;
		alarmCounter = NVIC_ST_CURRENT_R;
	}
	GPIO_PORTB_DATA_R ^= 0x04;	// Toggle PB2
	if(((alarmCounter - NVIC_ST_CURRENT_R)&0x0FFFFFFF) > (alarms[currentAlarmType].alarmTime[alarmCounterIndex]*80000)){
		alarmCounterIndex += 1;
		if(alarmCounterIndex == alarms[currentAlarmType].alarmLength) alarmCounterIndex = 0;
		TIMER2_TAILR_R = alarms[currentAlarmType].alarmSound[alarmCounterIndex];
		notePlaying = 0;
	}
}

// Initialize PB2 as a digital output for the speaker
void portBInit(void){
	SYSCTL_RCGCGPIO_R |= 0x02;            // activate port b
	GPIO_PORTB_DIR_R |= 0x04;							// PB2 output
	GPIO_PORTB_AFSEL_R &= ~0x04;					// Disable alternate function
	GPIO_PORTB_DEN_R |= 0x04;							// Enable digital I/0
}

// Initialize the speaker - the period will determine the tone that is played
void Speaker_Init(){
		portBInit();
		alarms[0] = alarmOne;
		alarms[1] = alarmTwo;
		alarms[2] = alarmThree;
		alarms[3] = alarmFour;
		alarmCounterIndex = 0;
		alarmCounter = alarms[currentAlarmType].alarmTime[alarmCounterIndex];
		Timer2_Init(&togglePB2, alarms[currentAlarmType].alarmSound[alarmCounterIndex]);
}

void Start_Alarm(void){
	alarmCounterIndex = 0;
	alarmCounter = alarms[currentAlarmType].alarmTime[alarmCounterIndex];
	alarmOn = 1;
}

void Stop_Alarm(void){
	alarmOn = 0;
}

// Increment to the next alarm type
void Change_Alarm(int x){
	currentAlarmType += x;
	if(currentAlarmType >= 4) currentAlarmType = 0;
	else if(currentAlarmType < 0) currentAlarmType = 3;
	
	notePlaying = 0;
	alarmCounterIndex = 0;
	TIMER2_TAILR_R = alarms[currentAlarmType].alarmSound[alarmCounterIndex];
}
