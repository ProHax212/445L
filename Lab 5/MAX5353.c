// MAX5353.c
// Runs on LM4F120/TM4C123
// Use SSI0 to send a 16-bit code to the MAX5353.
// Daniel Valvano
// September 11, 2013

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

// SSI0Clk (SCLK, pin 4) connected to PA2
// SSI0Fss (!CS, pin 2) connected to PA3
// SSI0Tx (DIN, pin 3) connected to PA5
// see Figure 7.19 for complete schematic
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"


//********DAC_Init*****************
// Initialize Max5353 12-bit DAC
// inputs:  initial voltage output (0 to 4095)
// outputs: none
// assumes: system clock rate less than 20 MHz
void DAC_Init(uint16_t data){
	SYSCTL_RCGCSSI_R |= 0x08;       // activate SSI3
  SYSCTL_RCGCGPIO_R |= 0x08;      // activate port D
  while((SYSCTL_PRGPIO_R&0x08) == 0){};// ready?
		
	GPIO_PORTD_AFSEL_R |= 0x0F;			// enable alt func on PD0-PD3
	GPIO_PORTD_DEN_R |= 0x0F;				// DEN for PD0-PD3
	GPIO_PORTD_DATA_R |= 0x02;			// PD1 FSS high
	GPIO_PORTD_PCTL_R = (GPIO_PORTD_PCTL_R&0xFFFF0000) + 0x00001111;	// Set the functions (SSI3) for PD0-PD3
  SSI3_CR1_R = 0x00000000;        // disable SSI, master mode
  SSI3_CPSR_R = 0x02;             // 8 MHz SSIClk 
  SSI3_CR0_R &= ~(0x0000FFF0);    // SCR = 0, SPH = 0, SPO = 0 Freescale
  SSI3_CR0_R |= 0x0F;             // DSS = 16-bit data
	//SSI0_DR_R = data;               // load 'data' into transmit FIFO
  SSI3_CR1_R |= 0x00000002;       // enable SSI
}



//********DAC_Out*****************
// Send data to Max5353 12-bit DAC
// inputs:  voltage output (0 to 4095)
// outputs: none
void DAC_Out(uint16_t code){   
  while((SSI3_SR_R&0x00000002)==0){};// SSI Transmit FIFO Not Full
  SSI3_DR_R = code; }                // data out, no reply
  
//********DAC_Out*****************
// Send data to Max5353 12-bit DAC
// inputs:  voltage output (0 to 4095)
// outputs: reply is returned
// send the 16-bit code to the SSI, return a reply
uint16_t DAC_Out2(uint16_t code){   uint16_t receive;
  while((SSI3_SR_R&0x00000002)==0){};// SSI Transmit FIFO Not Full
  SSI3_DR_R = code;                  // data out
  while((SSI3_SR_R&0x00000004)==0){};// SSI Receive FIFO Not Empty
  receive = SSI3_DR_R;               // acknowledge response
  return receive;
}

// Init Raw
// Initialize SSI3 (PD0 - PD3)
void DAC_InitRaw(void){
  SYSCTL_RCGCSSI_R |= 0x08;       // activate SSI3
  SYSCTL_RCGCGPIO_R |= 0x08;      // activate port D
  while((SYSCTL_PRGPIO_R&0x08) == 0){};// ready?
		
	GPIO_PORTD_AFSEL_R |= 0x0F;			// enable alt func on PD0-PD3
	GPIO_PORTD_DEN_R |= 0x0F;				// DEN for PD0-PD3
	GPIO_PORTD_DATA_R |= 0x02;			// PD1 FSS high
	GPIO_PORTD_PCTL_R = (GPIO_PORTD_PCTL_R&0xFFFF0000) + 0x00001111;	// Set the functions (SSI3) for PD0-PD3
  SSI3_CR1_R = 0x00000000;        // disable SSI, master mode
  SSI3_CPSR_R = 0x02;             // 8 MHz SSIClk 
  SSI3_CR0_R &= ~(0x0000FFF0);    // SCR = 0, SPH = 0, SPO = 0 Freescale
  SSI3_CR0_R |= 0x0F;             // DSS = 16-bit data
  SSI3_CR1_R |= 0x00000002;       // enable SSI
		
	// PORT A stuff
	/*GPIO_PORTA_AFSEL_R |= 0x24;     // enable alt funct on PA2,5, regular GPIO on PA3
  GPIO_PORTA_DEN_R |= 0x2C;       // configure PA2,5 as SSI, PA3 is regular GPIO
  GPIO_PORTA_DATA_R |= 0x08;      // PA3 FSS high
  GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R&0xFF0F00FF)+0x00200200;
  GPIO_PORTA_AMSEL_R = 0;         // disable analog functionality on PA*/

}
//********DAC_Out*****************
// Send data to Max5353 12-bit DAC
// inputs:  voltage output (0 to 4095)
// outputs: none
// send the 16-bit code to the SSI, 
void DAC_OutRaw(uint16_t code){
	GPIO_PORTD_DATA_R &= ~0x02;			// PD1 FSS low
	
	while((SSI3_SR_R&0x00000002)==0){};// SSI Transmit FIFO Not Full
	SSI3_DR_R = code;                  // data out

  GPIO_PORTD_DATA_R |= 0x02;         // PD1 FSS high
}
