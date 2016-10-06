// MAX5353TestMain.c
// Runs on LM4F120/TM4C123
// Test the functions provided in MAX5353.c by outputting
// a sine wave at a particular frequency.
// Daniel Valvano
// September 11, 2013

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2014

 Copyright 2014 by Jonathan W. Valvano, valvano@mail.utexas.edu
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

// SSI0Clk (SCLK, pin 4) connected to PA2
// SSI0Fss (!CS, pin 2) connected to PA3
// SSI0Tx (DIN, pin 3) connected to PA5
// see Figure 7.19 for complete schematic

#include <stdint.h>
#include "MAX5353.h"
#include "SysTick.h"
#include "music.h"
#include "Switch.h"
#include "Timer0A.h"
#include "Timer1.h"
#include "PLL.h"
#include "music2.h"

/* Get the amplitude to output and send to DAC
int currentAmplitude = 0;
void timerOneHandle(){
	int amplitude = Flute[currentAmplitude % 32];
	amplitude &= 0x0FFF;
	amplitude |= 0x6000;
	DAC_Out(amplitude);
	
	currentAmplitude += 1;
}*/

static int lastPressed = 0x11;	// None pressed
void checkInputs(){
	int buttonPress = Switch_Debounce();
	switch(buttonPress){
		// SW1 Pressed
		case 0x01:
			lastPressed = 0x01;
		  break;
		// SW2 Pressed
		case 0x10:
			lastPressed = 0x10;
			break;
		// SW3 Pressed
		case 0x100:
			lastPressed = 0x100;
			break;
		case 0x11:
			switch(lastPressed){
				case 0x01:
					lastPressed = 0x11;
					Toggle_Pause();
					break;
				case 0x10:
					lastPressed = 0x11;
					Rewind();
					break;
				case 0x100:
					lastPressed = 0x11;
					Switch_Instruments();
			}
			break;
	}
}

int main(void){
	PLL_Init(Bus20MHz);
  DAC_Init(0x1000);                  // initialize with command: Vout = Vref
  SysTick_Init();
	Switch_Init();
	Init_Music();
	
  while(1){
		checkInputs();
  }
}
