// LCDInterface.h

// Displays updated time, both digital and analog, to the LCD screen
// Input: hours, minutes
void LCD_Refresh_Time(uint8_t hour, uint8_t minute);

// Displays updated alarm to the LCD screen
// Input: hours, minutes
void LCD_Refresh_Alarm(uint8_t hour, uint8_t minute);

// Initializes the LCD screen, displays initial time
// Input: time hours, time minutes, alarm hours, alarm minutes
void LCD_Init(uint8_t timeHours, uint8_t timeMinutes, uint8_t alarmHours, uint8_t alarmMinutes);
