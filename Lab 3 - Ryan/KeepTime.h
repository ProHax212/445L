// KeepTime.h

// Output: time format
int Get_Time(void);

// Output: time format
int Get_Alarm(void);

// Input: time format
// Output: minutes
int Get_Minutes(int t);

// Input: time format
// Output: hours
int Get_Hours(int t);

// Input: time format
void Set_Time(int t);

// Input: time format
void Set_Alarm(int a);

// Starts the alarm if the current time equals the set alarm
void Check_Alarm(void);

// Input: time, seconds
// Output: time + seconds
int incrementTime(int t, int s);

// Initializes KeepTime
void KeepTime_Init(uint8_t timeHours, uint8_t timeMinutes, uint8_t alarmHours, uint8_t alarmMinutes);
