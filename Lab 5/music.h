#include <stdint.h>

const int notes = 1;
const int numSongs = 2;
const int waveSize = 32;

//Struct for instrument
typedef struct {
	int wavePointer;
	const uint16_t wave[waveSize];
}Instrument;

// Struct for parts
typedef struct {
	Instrument instrument;
	int notePointer;
	const int noteLength[notes];
	const int notePeriod[notes];
	int waitTime;
}Voice;

// Struct for the song
typedef struct {
	Voice voices[1];
}Song;

typedef struct {
	int songPointer;
	Song songs[numSongs];
}Player;

void skip(void);
void rewind(void);

//Returns amplitude of next note
int getAmp(void);

int getDelay(void);