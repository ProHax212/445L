#include <stdint.h>
#include "Timer1.h"
#include "Timer0A.h"
#include "..//inc//tm4c123gh6pm.h"
#include "MAX5353.h"

const unsigned short Flute[32] = {
	1007, 1252, 1374, 1548, 1698, 1797, 1825, 1797, 1675, 1562, 1383,
	1219, 1092, 1007, 913, 890, 833, 847, 810, 777, 744, 674,
	598, 551, 509, 476, 495, 533, 589, 659, 758, 876
};

const unsigned short Horn[32] = {
	1063, 1082, 1119, 1275, 1678, 1748, 1275, 755, 661, 661, 703,
	731, 769, 845, 1039, 1134, 1209, 1332, 1465, 1545, 1427, 1588,
	1370, 1086, 708, 519, 448, 490, 566, 684, 802, 992
};

const unsigned short Trumpet[32] = {
	1007, 1088, 1156, 1194, 1067, 789, 303, 99, 789, 1510, 1476,
	1173, 1067, 1037, 1084, 1062, 1011, 1015, 1045, 1062, 1011, 1011,
	1058, 1113, 1084, 1075, 1079, 1105, 1088, 1049, 1015, 1045
};

#define WN 16
#define HN 8
#define QN 4
#define EN 2
#define SN 1

#define R 0

#define A3 220.00
#define A3S 233.08
#define B3 246.94
#define C4 261.63
#define C4S 277.18
#define D4 293.66
#define D4S 311.13
#define E4 329.63
#define F4 349.23
#define F4S 369.99
#define G4 392.00
#define G4S 415.30
#define A4 440.00
#define A4S 466.16
#define B4 493.88
#define C5 523.25
#define C5S 554.37
#define D5 587.33

#define BPM 120

typedef struct{
	double frequencies[200];
	int durations[200];
	int songPointer;
	int songLength;
} Song;

/*Song song = {.frequencies={C4, D4, E4, F4, G4, A4, B4, C5},
.durations={QN, EN, EN, EN, EN, EN, EN, QN},
.songPointer=0,
.songLength = 8};*/

Song song = {.frequencies={F4S, E4, G4, F4S, D4, A3, F4S, E4, G4, F4S, D4, F4S, E4, E4, D4S, F4S, E4, C4S, E4, D4, C4S, D4, B3, C4S, E4, D4, A3, F4S, E4, G4, F4S, D4, A3, F4S, E4, G4, F4S, D4, F4S, E4, E4, D4S, F4S, E4, C4S, E4, D4, C4S, D4, B3, C4S, E4, D4, F4S, F4S, G4, B4, A4, F4S, G4, B4, A4, E4, G4, F4S, R, F4S, F4S, G4, A4, C5S, B4, A4, D4, C5S, B4, A4, D4, F4S, E4, D4, B3, D4, E4, F4S, G4, B4, A4, F4S, G4, B4, A4, E4, G4, F4S, R, F4S, F4S, G4, A4, C5S, B4, A4, D4, C5S, D5, A4, D4, F4S, E4, D4, E4, G4, F4S, D4, C4S, D4},
.durations={QN, EN, QN, EN, QN, WN, QN, EN, QN, EN, QN, HN, HN, QN, EN, QN, EN, QN, QN, QN, QN, QN, EN, QN, QN, QN, EN+WN, QN, EN, QN, EN, QN, WN, QN, EN, QN, EN, QN, HN, HN, QN, EN, QN, EN, QN, QN, QN, QN, QN, EN, QN, QN, QN, HN, EN, QN, QN, WN+QN+EN, EN, QN, QN, EN, QN, QN, EN, EN, HN+EN, QN, QN, QN, QN, QN, QN+EN, EN, QN, QN, QN+EN, EN, EN, QN, QN, EN, QN, QN+EN, EN, QN, QN, WN+QN+EN, EN, QN, QN, EN, QN, QN, EN, EN, HN+EN, QN, QN, QN, QN, QN, QN+EN, EN, QN, QN, QN+EN, EN, QN, QN, QN, QN, QN, QN, QN, QN, QN+HN+EN},
.songPointer = 0, .songLength = 117};

static int currentDuration = 0;
static int amplitudePointer = 0;
static int isPaused = 0;

// Flute - Trumpet - Horn
const unsigned short instruments[3][32] = {{1007, 1252, 1374, 1548, 1698, 1797, 1825, 1797, 1675, 1562, 1383,
	1219, 1092, 1007, 913, 890, 833, 847, 810, 777, 744, 674,
	598, 551, 509, 476, 495, 533, 589, 659, 758, 876},
{1007, 1088, 1156, 1194, 1067, 789, 303, 99, 789, 1510, 1476,
	1173, 1067, 1037, 1084, 1062, 1011, 1015, 1045, 1062, 1011, 1011,
	1058, 1113, 1084, 1075, 1079, 1105, 1088, 1049, 1015, 1045}, 
{1063, 1082, 1119, 1275, 1678, 1748, 1275, 755, 661, 661, 703,
	731, 769, 845, 1039, 1134, 1209, 1332, 1465, 1545, 1427, 1588,
	1370, 1086, 708, 519, 448, 490, 566, 684, 802, 992}};
static int instrumentPointer = 0;

// Next beat
void timerZeroHandle(){
	if(isPaused) return;
	
	if(currentDuration == 0){
		song.songPointer += 1;
		if(song.songPointer == song.songLength) song.songPointer = 0;
		TIMER1_TAILR_R = (20000000 / (song.frequencies[song.songPointer] * 32));
		currentDuration = song.durations[song.songPointer];
	}
	
	currentDuration -= 1;
}

// Output to the DAC
void timerOneHandle(){
	int amplitude = instruments[instrumentPointer][amplitudePointer % 32];
	//amplitude /= 4;
	amplitude &= 0x0FFF;
	amplitude |= 0x6000;
	
	DAC_Out(amplitude);
	
	amplitudePointer += 1;
	
}

static int lastFrequency;
// Play/Pause
void Toggle_Pause(){
	if(isPaused == 0){
		lastFrequency = TIMER1_TAILR_R;
		TIMER1_TAILR_R = 0;
		isPaused = 1;
	}
	else{
		TIMER1_TAILR_R = lastFrequency;
		isPaused = 0;
	}
}

// Rewind the song
void Rewind(){
	song.songPointer = 0;
	amplitudePointer = 0;
	currentDuration = song.durations[song.songPointer];
	
	if(isPaused) lastFrequency = (20000000 / (song.frequencies[song.songPointer] * 32));
	else TIMER1_TAILR_R = (20000000 / (song.frequencies[song.songPointer] * 32));
}

// Switch between the various instruments
void Switch_Instruments(){
	instrumentPointer = (instrumentPointer + 1) % 3;
}

void Init_Music(){
	currentDuration = song.durations[song.songPointer];
	Timer0A_Init(&timerZeroHandle, ((20000000 / (BPM / 60)) / 4));
	Timer1_Init(&timerOneHandle, (20000000 / (song.frequencies[song.songPointer] * 32)));
}
