// AlarmClockTestMain
// Ryan Comer
// September 14, 2016

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "PLL.h"
#include "Timer1.h"
#include "ST7735.h"
#include "KeepTime.h"
	
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

/*
Method for testing the line function
*/
void testDrawLine(void){
// Screen initialization
	ST7735_InitR(INITR_REDTAB);
	ST7735_FillScreen(0);  // set screen to black
  ST7735_SetCursor(0,0);
	ST7735_Line(64,64,64,0,ST7735_YELLOW);
	ST7735_Line(64,64,96,0,ST7735_YELLOW);
	ST7735_Line(64,64,127,0,ST7735_YELLOW);
	ST7735_Line(64,64,127,32,ST7735_YELLOW);
	ST7735_Line(64,64,127,64,ST7735_BLUE);

	ST7735_Line(64,64,96,127,ST7735_BLUE);
	ST7735_Line(64,64,64,127,ST7735_BLUE);
	ST7735_Line(64,64,32,127,ST7735_BLUE);

	ST7735_Line(64,64,0,64,ST7735_BLUE);
	ST7735_Line(64,64,0,32,ST7735_YELLOW);
	ST7735_Line(64,64,0,0,ST7735_YELLOW);
	ST7735_Line(64,64,32,0,ST7735_YELLOW);
}

int main(void){
  PLL_Init(Bus80MHz);	// 80 MHz
	
	KeepTime_Init();
	
  EnableInterrupts();
	
	while(1){
		
	}
	//testDrawLine();
	
}


