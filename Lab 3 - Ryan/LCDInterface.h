#include <stdint.h>
#include <math.h>
#include "ST7735.h"

void Draw_Hand(uint8_t deg, uint8_t length);

void Update_Clock(uint8_t timeHour, uint8_t timeMinute, uint8_t alarmHour, uint8_t alarmMinute);

void Draw_Face(void);

void LCDInit(void);
