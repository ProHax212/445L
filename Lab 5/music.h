#include <stdint.h>

const static int maxNotes = 100;
const static int maxChannels = 1;
const static int numSongs = 1;
const static int waveSize = 32;

//Struct for instrument
typedef struct {
	int wavePointer;
	const uint16_t wave[waveSize];
}Instrument;

typedef struct {
	int length;//Stoes number of beats for each note
	int period;//Stoes time between each wave update
}Note;

// Struct for parts
typedef struct {
	Note notes[maxNotes];
	Instrument *instrument;
	int notePointer;
	int beatCounter;
	int waitTime;
}Channel;

// Struct for the song
typedef struct {
	Channel *channels[maxChannels];
}Song;

typedef struct {
	Song *songs[numSongs];
	int songPointer;
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

// Initialize the player
void Init_Player(void);
