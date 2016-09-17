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
#include "Switch.h"
#include "Heartbeat.h"
#include "Speaker.h"
	
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

// Update the clock with the new time
void updateClock(){
	int minutes = Get_Minutes(), hours = Get_Hours(), alarmMinutes = Get_Alarm_Minutes(), alarmHours = Get_Alarm_Hours();
	Update_Clock(hours, minutes, alarmHours, alarmMinutes);
}

int main(void){
  PLL_Init(Bus80MHz);	// Initialize clock speed to 80MHz
	
	// Initialization
	LCD_Init();
	Board_Init();
	KeepTime_Init();
	PortF_Init();
	Heartbeat_Init();
	Speaker_Init(305775);
	
	EnableInterrupts();
	
	// Main loop
	while(1){
		Check_Inputs();
		updateClock();
		Check_Alarm();
	}
}





