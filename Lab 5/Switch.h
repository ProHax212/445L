//Switch.h

int Switch_Get_Play(void);
int Switch_Get_Skip(void);

// Check which switches are pressed and respond accordingly
// SW1 or SW2 pushed - stop alarm
// SW1 or SW2 released - send to Button_Released()
// Both pressed and released - reset
// Input: mode
void Check_Inputs(void);

//Initializes Board and PortF
void Switch_Init(void);
