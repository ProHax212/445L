#include <stdint.h>
#include "PLL.h"
#include "../inc/tm4c123gh6pm.h"
#include "Speaker.h"
#include "KeepTime.h"

#define SWITCHES                (*((volatile uint32_t *)0x40025044))
#define PF0   (*((volatile uint32_t *)0x40025004))
#define PF4   (*((volatile uint32_t *)0x40025040))
#define SW1       0x10                      // on the left side of the Launchpad board
#define SW2       0x01                      // on the right side of the Launchpad board

// Subroutine to wait 10 msec
// Inputs: None
// Outputs: None
// Notes: ...
void DelayWait10ms(uint32_t n){uint32_t volatile time;
  while(n){
    time = 727240*2/91;  // 10msec
    while(time){
	  	time--;
    }
    n--;
  }
}

uint8_t Pause(void){
  while(PF4==0x00){ 
    DelayWait10ms(10);
  }
  while(PF4==0x10){
    DelayWait10ms(10);
  }
	return 0;
}

// PF4 is input
// Make PF2 an output, enable digital I/O, ensure alt. functions off
void PortF_Init(void){ 
  SYSCTL_RCGCGPIO_R |= 0x20;        // 1) activate clock for Port F
  while((SYSCTL_PRGPIO_R&0x20)==0){}; // allow time for clock to start
                                 // 2a) unlock GPIO Port F Commit Register
  GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY;
  GPIO_PORTF_CR_R |= (SW1|SW2);  // 2b) enable commit for PF4 and PF0
  GPIO_PORTF_PCTL_R &= ~0x000F0F00; // 3) regular GPIO
  GPIO_PORTF_AMSEL_R &= ~0x14;      // 4) disable analog function on PF2, PF4
  GPIO_PORTF_PUR_R |= 0x10;         // 5) pullup for PF4
  GPIO_PORTF_DIR_R |= 0x04;         // 5) set direction to output
  GPIO_PORTF_AFSEL_R &= ~0x14;      // 6) regular port function
  GPIO_PORTF_DEN_R |= 0x14;         // 7) enable digital port
}

//------------Board_Init------------
// Initialize GPIO Port F for negative logic switches on PF0 and
// PF4 as the Launchpad is wired.  Weak internal pull-up
// resistors are enabled, and the NMI functionality on PF0 is
// disabled.
// Input: none
// Output: none
void Board_Init(void){            
  SYSCTL_RCGCGPIO_R |= 0x20;     // 1) activate Port F
  while((SYSCTL_PRGPIO_R&0x20) == 0){};// ready?
                                 // 2a) unlock GPIO Port F Commit Register
  GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY;
  GPIO_PORTF_CR_R |= (SW1|SW2);  // 2b) enable commit for PF4 and PF0
                                 // 3) disable analog functionality on PF4 and PF0
  GPIO_PORTF_AMSEL_R &= ~(SW1|SW2);
                                 // 4) configure PF0 and PF4 as GPIO
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFF0FFF0)+0x00000000;
  GPIO_PORTF_DIR_R &= ~(SW1|SW2);// 5) make PF0 and PF4 in (built-in buttons)
                                 // 6) disable alt funct on PF0 and PF4
  GPIO_PORTF_AFSEL_R &= ~(SW1|SW2);
//  delay = SYSCTL_RCGC2_R;        // put a delay here if you are seeing erroneous NMI
  GPIO_PORTF_PUR_R |= (SW1|SW2); // enable weak pull-up on PF0 and PF4
  GPIO_PORTF_DEN_R |= (SW1|SW2); // 7) enable digital I/O on PF0 and PF4
}

//------------Board_Input------------
// Read and return the status of the switches.
// Input: none
// Output: 0x01 if only Switch 1 is pressed
//         0x10 if only Switch 2 is pressed
//         0x00 if both switches are pressed
//         0x11 if no switches are pressed
uint32_t Board_Input(void){
  return SWITCHES;
}

// Set the time of the clock
// SW1 increments the current step by 1
// SW2 moves to the next step (Hours, Minutes, Done)
void setTime(){
	int minutes = Get_Minutes();
	int hours = Get_Hours();
	
	// Set hours
	while(1){
		int status = Board_Input();
		if(status == 0x01){	// SW1 pressed
			hours += 1;
			if(hours == 24) hours = 0;
		}
		else if(status == 0x10) break;	// SW2 pressed - move to minutes
	}
	
	// Set minutes
	while(1){
		int status = Board_Input();
		if(status == 0x01){	// SW1 pressed
			minutes += 1;
			if(minutes == 60) minutes = 0;
		}
		else if(status == 0x10) break;	// SW2 pressed - done
	}
	
	Set_Time(hours, minutes);
}

// Set the time of the clock
// SW1 increments the current step by 1
// SW2 moves to the next step (Hours, Minutes, Done)
void setAlarm(){
	
	int minutes = Get_Alarm_Minutes();
	int hours = Get_Alarm_Hours();
	
	// Set hours
	while(1){
		int status = Board_Input();
		if(status == 0x01){	// SW1 pressed
			hours += 1;
			if(hours == 24) hours = 0;
		}
		else if(status == 0x10) break;	// SW2 pressed - move to minutes
	}
	
	// Set minutes
	while(1){
		int status = Board_Input();
		if(status == 0x01){	// SW1 pressed
			minutes += 1;
			if(minutes == 60) minutes = 0;
		}
		else if(status == 0x10) break;	// SW2 pressed - done
	}
	
	Set_Alarm(hours, minutes);
}

// Check which switches are pressed and go to the proper state
// SW1 pressed - (Stop alarm/Set time) - start counting to 2 seconds
// SW2 pressed - Set alarm - start counting to 2 seconds
int static setTimeCounter = 0;
int static setAlarmCounter = 0;
void Check_Inputs(void){
	int status = Board_Input();
	switch (status){
		case 0x01: // SW1 pressed
			StopAlarm();
			setTimeCounter += 1;
		
			// 0x13 represents 2 seconds
			if(setTimeCounter == 0x13){
				setTime();
			}
			break;
    case 0x10: // SW2 pressed
			setAlarmCounter += 1;
		
			// 0x13 represents 2 seconds
			if(setAlarmCounter == 0x13){
				setAlarm();;
			}
			break;
    case 0x00: // both switches pressed
			break;
    case 0x11: // neither switch pressed
			setTimeCounter = 0;
			setAlarmCounter = 0;
		break;
	}
}
