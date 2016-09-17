//KeepTime.h

static int currentTime;
static int currentAlarm;

// Get the current minutes for the time
int Get_Minutes(void);

// Get the current hour for the time
int Get_Hours(void);

// Get the current minutes for the alarm
int Get_Alarm_Minutes(void);

// Get the current hours for the alarm
int Get_Alarm_Hours(void);

// Set the current time
void Set_Time(int h, int m);

// Set the current alarm
void Set_Alarm(int h, int m);

//Adds 1 minute to a time
int incrementTime(int t);

//Initializes KeepTime
void KeepTime_Init(void);

// Starts the alarm if the current time equals the set alarm
void Check_Alarm(void);
