//KeepTime.h

static int currentTime;
static int currentAlarm;

//Input: minutes, Output: time format
int getMinutes(int t);

//Input: hours, Output: time format
int getHours(int t);

//Adds 1 minute to a time
int incrementTime(int t);

//Initializes KeepTime
void KeepTime_Init(void);
