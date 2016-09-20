//Switch.h

// Check which switches are pressed and respond accordingly
// SW1 or SW2 pushed - stop alarm
// SW1 or SW2 released - send to Button_Released()
// Both pressed and released - reset
// Input: mode
void Check_Inputs(int m);

//Initializes Board and PortF
void Switch_Init(void);
