#include <stdint.h>
#include "music.h"

    // calculated frequencies are not exact, due to the impreciseness of delay loops
    // assumes using 16 MHz PIOSC (default setting for clock source)
    // maximum frequency with 16 MHz PIOSC: (8,000,000 bits/1 sec)*(1 sample/16 bits)*(1 wave/32 sample) = 15,625 Hz
    // maximum frequency with 20 MHz PLL: (10,000,000 bits/1 sec)*(1 sample/16 bits)*(1 wave/32 sample) = 19,531.25 Hz
//    SysTick_Wait(0);                 // ?? kHz sine wave (actually 12,000 Hz)
//    SysTick_Wait(9);                 // 55.6 kHz sine wave (actually 10,000 Hz)
//    SysTick_Wait(15);                // 33.3 kHz sine wave (actually 8,500 Hz)
//    SysTick_Wait(19);                // 26.3 kHz sine wave (actually 8,500 Hz)
//    SysTick_Wait(64);                // 7.81 kHz sine wave (actually 4,800 Hz)
//    SysTick_Wait(99);                // 5.05 kHz sine wave (actually 3,500 Hz)
//    SysTick_Wait(1136);              // 440 Hz sine wave (actually 420 Hz)
//    SysTick_Wait(50000);             // 10 Hz sine wave (actually 9.9 Hz)

// 12-bit 32-element sine wave
// multiply each value by 2 to shift into bits 12:1 of SSI packet
// three control bits in 15:13 are all zero for immediate DAC update
// book figure shows MAX5353 in unipolar rail-to-rail configuration
// that means when wave[n] = 0x0000 (LSB = 0), output = 0
//                 wave[n] = 0x1000 (LSB = 0), output = Vref
//                 wave[n] = 0x1FFE (LSB = 0), output = 2*Vref
const Instrument sine = {.wavePointer=0, .wave = {
  2048*2,2448*2,2832*2,3186*2,3496*2,3751*2,3940*2,4057*2,4095*2,4057*2,3940*2,
  3751*2,3496*2,3186*2,2832*2,2448*2,2048*2,1648*2,1264*2,910*2,600*2,345*2,
  156*2,39*2,0*2,39*2,156*2,345*2,600*2,910*2,1264*2,1648*2}};

// 32 outputs to the DAC for a flute sound
const Instrument flute = {.wavePointer=0, .wave = {
	1007, 1252, 1374, 1548, 1698, 1797, 1825, 1797, 1675, 1562, 1383, 1219, 1092, 1007, 913, 890, 833, 847, 
	810, 777, 744, 674, 598, 551, 509, 476, 495, 533, 589, 659, 758, 876}};

Player player;

void rewind(){
	Song song = player.songs[player.songPointer];
	for(int v=0; v<sizeof(song.voices); v++){//For all voices
		song.voices[v].notePointer = 0;//Reset to note 0
		song.voices[v].instrument.wavePointer = 0;//Reset wave to 0
	}
	player.songs[player.songPointer] = song;//Save song
}

void skip(){
	rewind();//Reset current song
	player.songPointer++;//Go to next song
}

//Returns current amplitude
int getAmp(){
	Song song = player.songs[player.songPointer];
	int amp = 0;
	for(int v=0; v<sizeof(song.voices); v++){//For each voice
		Voice voice = song.voices[v];
		amp += voice.instrument.wave[voice.instrument.wavePointer];//Add to the amplitude
		
		if(voice.waitTime == 0){//Ready for next wave point
			voice.instrument.wavePointer++;//Increment wave pointer, update wait time
			voice.waitTime = voice.notePeriod[voice.notePointer];
		}
		if(voice.instrument.wavePointer == waveSize){//End of wave
			voice.notePointer++;//Next note
			voice.instrument.wavePointer = 0;//Restart wave
		}
		song.voices[v] = voice;//Save voice
	}
	player.songs[player.songPointer] = song;//Save song
	
	return (amp/sizeof(song.voices))&0x1F;//Average to prevent too high of amplitude
}

//Returns delay until next wave update
int getDelay(){
	Song song = player.songs[player.songPointer];
	int lowestWait = song.voices[0].waitTime;
	for(int v=0; v<sizeof(song.voices); v++){//For each voice
		if(song.voices[v].waitTime < lowestWait)
			lowestWait = song.voices[v].waitTime;//Update lowest wait time
	}
	for(int v=0; v<sizeof(song.voices); v++){//For each voice
		song.voices[v].waitTime -= lowestWait;//Subtract lowest wait time
	}
	return lowestWait;
}

void Init_Player(){
	//Initialize voices
	Voice singleSineNote = {.instrument=sine, .noteLength={1000},  .notePeriod={1136}};
	Voice singleFluteNote = {.instrument=flute, .noteLength={1000}, .notePeriod={1136}};
	
	//Initialize player
	player.songPointer = 0;
	
	//Add songs to player
	player.songs[0].voices[0] = singleSineNote;//Define voices of each song
	player.songs[1].voices[0] = singleFluteNote;
	
	for(int s=0; s<numSongs; s++){//Set to beginning of all songs
		player.songPointer = 0;
		rewind();
	}
}