// AlarmClockTestMain
// Ryan Comer
// September 14, 2016

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "PLL.h"
#include "Timer1.h"
#include "ST7735.h"
#include "KeepTime.h"
#include "LCDInterface.h"
#include "Heartbeat.h"
#include "Speaker.h"
#include "Switch.h"
	
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

static int mode;
static int savedTime;
static int savedAlarm;

// Input: mode
void Set_Mode(int m){
	mode = m;
	if(mode == 0){
		Set_Time(savedTime);
		Set_Alarm(savedAlarm);
	} else if(mode == 3){	// Customize alarm
		Start_Alarm();
	}else {
		savedTime = Get_Time();
		savedAlarm = Get_Alarm();
	}
}

// Don't update the time if setting mode from customization
void Set_Mode_Customize(int m){
	mode = m;
}

// Updates the time if in 'Normal' mode (mode 0)
void Clock_Tick(int time, int alarm){
	if(mode == 0){
		LCD_Refresh_Time(Get_Hours(time), Get_Minutes(time));
	}
}

// Updates the time in 'Time Set' mode (mode 1)
void Change_Clock_Time(int s){
	savedTime = incrementTime(savedTime, s);
	LCD_Refresh_Time(Get_Hours(savedTime), Get_Minutes(savedTime));
}

// Updates the alarm in 'Alarm Set mode (mode 2)
void Change_Clock_Alarm(int s){
	savedAlarm = incrementTime(savedAlarm, s);
	LCD_Refresh_Alarm(Get_Hours(savedAlarm), Get_Minutes(savedAlarm));
}

int main(void){
  PLL_Init(Bus80MHz);	// Initialize clock speed to 80MHz
	
	mode = 0;
	
	uint8_t timeHours = 0;
	uint8_t timeMinutes = 0;
	uint8_t alarmHours = 1;
	uint8_t alarmMinutes = 0;
	
	// Initialization
	LCD_Init(timeHours, timeMinutes, alarmHours, alarmMinutes);
	Switch_Init();
	KeepTime_Init(timeHours, timeMinutes, alarmHours, alarmMinutes);
	Heartbeat_Init();
	Speaker_Init();
	
	EnableInterrupts();
	
	// Main loop
	while(1){
		Check_Inputs(mode);
	}
}
