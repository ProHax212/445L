#include <stdint.h>

const int maxNotes = 2;
const int maxChannels = 2;
const int numSongs = 2;
const int waveSize = 32;

//Struct for instrument
typedef struct {
	int wavePointer;
	const uint16_t wave[waveSize];
}Instrument;

typedef struct {
	const int length;//Stoes number of beats for each note
	const int period;//Stoes time between each wave update
}Note;

// Struct for parts
typedef struct {
	Instrument instrument;
	const Note notes[2];
	int notePointer;
	int beatCounter;
	int waitTime;
}Channel;

// Struct for the song
typedef struct {
	Channel channels[maxChannels];
}Song;

typedef struct {
	int songPointer;
	Song songs[numSongs];
}Player;

//Switches to next repeat setting
void Music_Increment_Repeat(void);

//Resets current song to beginning
void Music_Rewind(void);

//Resets current song, handles next song based on repeat settings
void Music_Skip(void);

//Returns current amplitude
//Loops through wave once it reaches the end
int Music_Update_Amplitude(void);

//Returns the wait time until the next update
//Updates wait times for each channel
int Music_Get_Wait(void);

//Increment song to next beat
void Music_Next_Beat(void);