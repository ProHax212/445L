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
	
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

int main(void){
  PLL_Init(Bus80MHz);	// Initialize clock speed to 80MHz
	
	// Initialization
	LCD_Init();
	KeepTime_Init();
	PortF_Init();
	Heartbeat_Init();
	
	EnableInterrupts();
	
	// Main loop
	while(1){	
		Update_Clock(getHours(currentTime), getMinutes(currentTime), getHours(currentAlarm), getMinutes(currentAlarm));
		if(Pause() == 0)
			currentTime = incrementTime(currentTime);
		else
			currentAlarm = incrementTime(currentAlarm);
	}
}





