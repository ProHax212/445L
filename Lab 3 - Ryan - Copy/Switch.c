#include <stdint.h>
#include "PLL.h"
#include "../inc/tm4c123gh6pm.h"
#include "Speaker.h"
#include "KeepTime.h"
#include "AlarmClockTestMain.h"

#define SWITCHES  (*((volatile uint32_t *)0x40025044))
#define PF0   		(*((volatile uint32_t *)0x40025004))
#define PF4   		(*((volatile uint32_t *)0x40025040))
#define SW1       0x10                      // on the left side of the Launchpad board
#define SW2       0x01                      // on the right side of the Launchpad board
#define HOUR 			3600
#define MIN				60
#define DELAY10US 800	// 10 microseconds for a 80MHz clock
#define TWOSECONDS	0xBD	// 2 seconds waiting

// Initialize SysTick with busy wait running at bus clock.
void SysTick_Init(void){
  NVIC_ST_CTRL_R = 0;                   // disable SysTick during setup
  NVIC_ST_RELOAD_R = NVIC_ST_RELOAD_M;  // maximum reload value
  NVIC_ST_CURRENT_R = 0;                // any write to current clears it
                                        // enable SysTick with core clock
  NVIC_ST_CTRL_R = NVIC_ST_CTRL_ENABLE+NVIC_ST_CTRL_CLK_SRC;
}

// Time delay using busy wait.
// The delay parameter is in units of the core clock. (units of 62.5 nsec for 16 MHz clock)
void SysTick_Wait(uint32_t delay){
  volatile uint32_t elapsedTime;
  uint32_t startTime = NVIC_ST_CURRENT_R;
  do{
    elapsedTime = (startTime-NVIC_ST_CURRENT_R)&0x00FFFFFF;
  }
  while(elapsedTime <= delay);
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

//------------Switch_Debounce------------
// Read and return the status of the switches 
// Input: none
// Output: 0x01 SW1, 0x10 SW2, 0x00 Both
// debounces switch
uint32_t Switch_Debounce(void){
uint32_t in,old,time; 
  time = 1000; // 10 ms
  old = Board_Input();
  while(time){
    SysTick_Wait(DELAY10US); // 10us
    in = Board_Input();
    if(in == old){
      time--; // same value 
    }else{
      time = 1000;  // different
      old = in;
    }
  }
  return old;
}

// Handles button release actions
// SW1 - Set Time/Increment Time Hours/Increment Alarm Hours (mode 1/mode2/mode3)
// SW2 - Set Alarm/Increment Time Minutes/Increment Alarm Minutes (mode 1/mode2/mode3)
// Input: button number, mode
void Button_Released(int button, int m){
		switch (button){
		case 1: // SW1 released
			if(m == 0){//Normal Mode
				Set_Mode(1);
				break;
			} else if(m == 1) {
				Change_Clock_Time(HOUR);
				break;
			} else if(m == 2) {
				Change_Clock_Alarm(HOUR);
				break;
			} else if(m == 3){
				Change_Alarm(-1);
				break;
			}
			break;
    case 2: // SW2 released
			if(m == 0){
				Set_Mode(2);
				break;
			} else if(m == 1){
				Change_Clock_Time(MIN);
				break;
			} else if(m == 2){
				Change_Clock_Alarm(MIN);
				break;
			} else if(m == 3){
				Change_Alarm(1);
				break;
			}
			break;
		case 3:	// Both released
			if(m == 0){
				Set_Mode(3);
			}
	}
}

// Check which switches are pressed and respond accordingly
// SW1 or SW2 pushed - stop alarm
// SW1 or SW2 released - send to Button_Released()
// Both pressed and released - reset, Choose alarm type (hold for 2 seconds)
// Input: mode
static int pressed;
static int bothPressed = 0;	// Need a different variable for both to keep from overwriting
static int reset;
static int switchOneTimer = 0;	// Need to hold for 2 seconds
static int switchTwoTimer = 0;  // Need to hold for 2 seconds
static int switchBothTimer = 0;	// Need to hold for 2 seconds
void Check_Inputs(int m){
	int status = Switch_Debounce();
	switch (status){
		case 0x01: //SW1 pressed
			if(m == 0) Stop_Alarm();
			pressed = 1;
			if(m == 0){	// Normal mode - wait for 2 seconds
				switchOneTimer += 1;
				if(switchOneTimer < TWOSECONDS) pressed = 0;
			}
			break;
		case 0x10: //SW2 pressed
			if (m == 0) Stop_Alarm();
			pressed = 2;
			if(m == 0){	// Normal mode - wait for 2 seconds
				switchTwoTimer += 1;
				if(switchTwoTimer < TWOSECONDS) pressed = 0;
			}
			break;
		case 0x00: //Both pressed
			if(m == 0){	// Normal mode - wait for 2 seconds
				switchBothTimer += 1;
				bothPressed = 1;
				if(switchBothTimer < TWOSECONDS) bothPressed = 0;
			}else if(m == 3){
				Stop_Alarm();
				Set_Mode_Customize(0);
			}else reset = 1;	// Only reset if mode isn't normal or in customization
			break;
		case 0x11: //Neither pressed
			switchOneTimer = 0;
			switchTwoTimer = 0;
			switchBothTimer = 0;
			if(reset == 1){
				reset = 0;
				Stop_Alarm();
				Set_Mode(0);
			} else if(bothPressed){
				bothPressed = 0;
				Button_Released(3, m);	// Hardcode in a value of 3 for when both are pressed
			}else if(pressed == 1 || pressed == 2){
				Button_Released(pressed, m);
			}
			pressed = 0;
			break;
	}
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

// Initializes Board and PortF
void Switch_Init(){
	Board_Init();
	PortF_Init();
	SysTick_Init();
}
