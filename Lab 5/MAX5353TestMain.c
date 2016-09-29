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

int main(void){
  uint32_t i=0;
  DAC_Init(0x1000);                  // initialize with command: Vout = Vref
  SysTick_Init();
	Switch_Init();
	
  while(1<2){
		Check_Inputs();
		if(Switch_Get_Skip() == 1)
			skip();
		if(Switch_Get_Play() == 1){
			DAC_Out(getAmp());
			SysTick_Wait(getDelay());
		}
  }
}
