// ADCTestMain.c
// Runs on TM4C123
// This program periodically samples ADC channel 0 and stores the
// result to a global variable that can be accessed with the JTAG
// debugger and viewed with the variable watch feature.
// Daniel Valvano
// September 5, 2015

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2015

 Copyright 2015 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

// center of X-ohm potentiometer connected to PE3/AIN0
// bottom of X-ohm potentiometer connected to ground
// top of X-ohm potentiometer connected to +3.3V 
#include <stdint.h>
#include "ADCSWTrigger.h"
#include "../inc/tm4c123gh6pm.h"
#include "PLL.h"
#include "Timer1.h"
#include "ST7735.h"

#define PF2             (*((volatile uint32_t *)0x40025010))
#define PF1             (*((volatile uint32_t *)0x40025008))
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

const int NUM_READINGS = 1000;
const int ADC_RANGE = 4096;

volatile uint32_t ADCvalue;
// This debug function initializes Timer0A to request interrupts
// at a 100 Hz frequency.  It is similar to FreqMeasure.c.
void Timer0A_Init100HzInt(void){
  volatile uint32_t delay;
  DisableInterrupts();
  // **** general initialization ****
  SYSCTL_RCGCTIMER_R |= 0x01;      // activate timer0
  delay = SYSCTL_RCGCTIMER_R;      // allow time to finish activating
  TIMER0_CTL_R &= ~TIMER_CTL_TAEN; // disable timer0A during setup
  TIMER0_CFG_R = 0;                // configure for 32-bit timer mode
  // **** timer0A initialization ****
                                   // configure for periodic mode
  TIMER0_TAMR_R = TIMER_TAMR_TAMR_PERIOD;
  TIMER0_TAILR_R = 799999;         // start value for 100 Hz interrupts
  TIMER0_IMR_R |= TIMER_IMR_TATOIM;// enable timeout (rollover) interrupt
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;// clear timer0A timeout flag
  TIMER0_CTL_R |= TIMER_CTL_TAEN;  // enable timer0A 32-b, periodic, interrupts
  // **** interrupt initialization ****
                                   // Timer0A=priority 2
  NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF)|0x40000000; // top 3 bits
  NVIC_EN0_R = 1<<19;              // enable interrupt 19 in NVIC
}

// Arrays for data/time paris
static int times[NUM_READINGS];
static int data[NUM_READINGS];
static int currentIndex = 0;

void Timer0A_Handler(void){
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;    // acknowledge timer0A timeout
  PF2 ^= 0x04;                   // profile
  PF2 ^= 0x04;                   // profile
  ADCvalue = ADC0_InSeq3();
	
	// Dump the data
	if(currentIndex < NUM_READINGS){
		times[currentIndex] = TIMER1_TAR_R;
		data[currentIndex] = ADCvalue;
		currentIndex += 1;
	}
	
  PF2 ^= 0x04;                   // profile
}

/*
Find the time jitter (highest time difference - smallest time difference)
Assumes that the times array has been filled
*/
int findJitter(){
	int smallestTimeDifference = (times[1] - times[0]);
	int largestTimeDifference = (times[1] - times[0]);
	
	// Loop through the times array and find the smallest and largest time differences
	int i = 0;
	for(i = 1; i < NUM_READINGS; i++){
		int difference = times[i] - times[i - 1];
		
		// Replace the smallest/largest time difference if necessary
		if(difference < smallestTimeDifference) smallestTimeDifference = difference;
		if(difference > largestTimeDifference) largestTimeDifference = difference;
	}
	
	return largestTimeDifference - smallestTimeDifference;
}

/*
Create a graph based on the values in data
*/
static int occurances[ADC_RANGE];
void plot(void){
	// The minimum and maximum ADC values read
	int maxValue = data[0];
	int minValue = maxValue;
	
	// First pass: get range of x values (Find the min and max adc value)
	for(int i=1; i<NUM_READINGS; i++){
		if(maxValue < data[i]) maxValue = data[i];
		if(minValue > data[i]) minValue = data[i];
	}
	
	// Now you only have to check from index [minValue, maxValue]
	
	// Second pass: get # of occurances for each value
	// ADC value = index in the occurances array
	int mode = 0;
	for(int i=minValue; i<=maxValue; i++){
		occurances[data[i]] += 1;
		if(occurances[data[i]] > mode) mode = occurances[data[i]];
	}
	
	//Third pass: get the range of y values (min and max occurences)
	int minOccurences = occurances[minValue], maxOccurences = occurances[minValue];
	for(int i=minValue; i<=maxValue; i++){
		if(occurances[i] < minOccurences) minOccurences = occurances[i];
		if(occurances[i] > maxOccurences) maxOccurences = occurances[i];
	}
		
	// Screen initialization
	ST7735_InitR(INITR_REDTAB);
	ST7735_FillScreen(0);  // set screen to black
  ST7735_SetCursor(0,0);
	ST7735_OutString("ADC PMF");
	ST7735_PlotClear(0, maxOccurences);	// maxOccurence is the maximum y value (scale the plot)
	
	// Loop over the occurences and plot their values
	for(int i = minValue; i <= maxValue; i++){
		ST7735_PlotBar(occurances[i]);
		ST7735_PlotNext();
	}
}
/*
	ST7735_SetCursor();     
	ST7735_OutUDec();     
	ST7735_OutString();     
	ST7735_PlotClear();     
	ST7735_PlotBar();     
	ST7735_PlotNext();
	
	for(int i=0; i<NUM_READINGS; i++){//for each x value from min to max
		int x = 128*(i)/(max-min);//Scale x value
		int h = 160*occurances[i]/mode;//Scale bar height
		int y = 160;
		ST7735_DrawFastVLine(x, y, h, ST7735_BLUE);
	}
}*/
int main(void){
  PLL_Init(Bus8
0MHz);                   // 80 MHz
	Timer1_Init();												// Initialize the timer for keeping data/time pairs
  SYSCTL_RCGCGPIO_R |= 0x20;            // activate port F
  ADC0_InitSWTriggerSeq3_Ch9();         // allow time to finish activating
  Timer0A_Init100HzInt();               // set up Timer0A for 100 Hz interrupts
  GPIO_PORTF_DIR_R |= 0x06;             // make PF2, PF1 out (built-in LED)
  GPIO_PORTF_AFSEL_R &= ~0x06;          // disable alt funct on PF2, PF1
  GPIO_PORTF_DEN_R |= 0x06;             // enable digital I/O on PF2, PF1
                                        // configure PF2 as GPIO
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFFF00F)+0x00000000;
  GPIO_PORTF_AMSEL_R = 0;               // disable analog functionality on PF
  PF2 = 0;                      // turn off LED
  EnableInterrupts();
  while(currentIndex < NUM_READINGS){
    PF1 ^= 0x02;  // toggles when running in main
  }
	
	// Calculate the time jitter for all of the recordings
	int timeJitter = findJitter();
	
	// Draw the graph for the data
	plot();
	
}


