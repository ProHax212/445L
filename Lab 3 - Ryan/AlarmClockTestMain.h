//AlarmClockTestMain.h

// Input: mode
void Set_Mode(int m);

// Updates the time if in 'Normal' mode (mode 0)
void Clock_Tick(int time, int alarm);

// Updates the time in 'Time Set' mode (mode 1)
void Change_Clock_Time(int s);

// Updates the alarm in 'Alarm Set mode (mode 2)
void Change_Clock_Alarm(int s);

void Set_Mode_Customize(int m);
