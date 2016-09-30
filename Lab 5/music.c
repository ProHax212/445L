#include <stdint.h>
#include <stdlib.h>
#include "music.h"

#define SN 1
#define EN 2
#define DEN 3
#define QN 4
#define DQN 6
#define HN 8
#define DHN 12
#define WN 16
#define DWN 24

const Note nullNotes[maxNotes] = {{0,0}, {0,0}};//Default channel if using less than maxChannels
const Note singleNote[maxNotes] = {{WN,1000}, {0,0}};//Extra null notes if using less than maxNotes
const Note doubleNote[maxNotes] = {{HN, 1200}, {HN, 1200}};

//32 outputs for a sine wave
const Instrument sine = {.wavePointer=0, .wave = {
  2048*2,2448*2,2832*2,3186*2,3496*2,3751*2,3940*2,4057*2,4095*2,4057*2,3940*2,
  3751*2,3496*2,3186*2,2832*2,2448*2,2048*2,1648*2,1264*2,910*2,600*2,345*2,
  156*2,39*2,0*2,39*2,156*2,345*2,600*2,910*2,1264*2,1648*2}};

// 32 outputs for a flute sound
const Instrument flute = {.wavePointer=0, .wave = {
	1007*2, 1252*2, 1374*2, 1548*2, 1698*2, 1797*2, 1825*2, 1797*2, 1675*2, 1562*2, 1383*2, 1219*2, 1092*2, 1007*2, 913*2, 890*2, 833*2, 847*2, 
	810*2, 777*2, 744*2, 674*2, 598*2, 551*2, 509*2, 476*2, 495*2, 533*2, 589*2, 659*2, 758*2, 876*2}};

Player player;
int repeat;//0-no repeat, 1-repeat, 2-shuffle

//Switches to next repeat setting
void Music_Increment_Repeat(){
	repeat++;
	repeat%=3;
}	

//Resets current song to beginning
void Music_Rewind(){
	Song song = player.songs[player.songPointer];
	for(int c=0; c<sizeof(song.channels); c++){//For all channels
		Channel chan = song.channels[c];
		chan.notePointer = 0;//Reset channel to note 0
		chan.waitTime = chan.notes[chan.notePointer].period;//Reset wait time to note 0
		chan.instrument.wavePointer = 0;//Reset wave to 0
		song.channels[c] = chan;//Save channel
	}
	player.songs[player.songPointer] = song;//Save song
}

//Resets current song, handles next song based on repeat settings
void Music_Skip(){
	Music_Rewind();//Reset current song
	switch(repeat){
		case 0://Next
			player.songPointer++;
			break;
		case 1://Repeat
			break;
		case 2://Shuffle
			player.songPointer = rand() % sizeof(player.songs);
			break;
	}
}

//Returns current amplitude
//Loops through wave of current note
int Music_Update_Amplitude(){
	Song song = player.songs[player.songPointer];
	int amp = 0;
	
	for(int c=0; c<sizeof(song.channels); c++){//For each channel
		Channel channel = song.channels[c];
		
		if(channel.notePointer < sizeof(channel.notes)){//If channel is not finished
			amp += channel.instrument.wave[channel.instrument.wavePointer];//Add to the amplitude
			
			if(channel.waitTime <= 0){//Ready for next wave point (should never be < 0)
				channel.instrument.wavePointer++;//Increment wave pointer
				channel.instrument.wavePointer %= waveSize;//Loop wave if necessary
				channel.waitTime += channel.notes[channel.notePointer].period;//Update wait time to next point on wave
			}
			song.channels[c] = channel;//Save channel
		}
	}
	player.songs[player.songPointer] = song;//Save song
	
	return (amp/sizeof(song.channels));//Average to prevent too high of amplitude
}

//Returns the wait time until next note
//Updates wait times of each channel
int Music_Get_Wait(){
	Song song = player.songs[player.songPointer];
	int lowestWait = song.channels[0].waitTime;
	for(int c=0; c<sizeof(song.channels); c++){//For each channel
		if(song.channels[c].waitTime < lowestWait)
			lowestWait = song.channels[c].waitTime;//Update lowest wait time
	}
	for(int c=0; c<sizeof(song.channels); c++){//For each channel
		song.channels[c].waitTime -= lowestWait;//Subtract lowest wait time
	}
	player.songs[player.songPointer] = song;//Save song
	return lowestWait;
}

//Increment song to next beat
void Music_Next_Beat(){
	Song song = player.songs[player.songPointer];
	
	for(int c=0; c<sizeof(song.channels); c++){//For each channel
		Channel chan = song.channels[c];
		chan.beatCounter++;//Increment beat
		if(chan.beatCounter >= chan.notes[chan.notePointer].length){//If last beat of note
			chan.beatCounter = 0;//Go to next note
			chan.notePointer++;
		}
		song.channels[c] = chan;//Save channel
	}
	player.songs[player.songPointer] = song;//Save song
}

void Init_Player(){
	//Initialize channels
	Channel singleSineNote = {sine, *singleNote};
	Channel singleFluteNote = {flute, *singleNote};
	Channel doubleSineNote = {sine, *doubleNote};
	Channel nullChannel = {sine, *nullNotes};
	
	//Initialize player
	player.songPointer = 0;
	
	//Add songs to player
	player.songs[0].channels[0] = singleSineNote;//Define channels of each song
	player.songs[0].channels[1] = nullChannel;//Add null channel if only using one channel
	
	player.songs[1].channels[0] = singleFluteNote;
	player.songs[1].channels[1] = doubleSineNote;
	
	player.songPointer = 0;//Set to first song
	for(int s=0; s<numSongs; s++)//Set all songs to the beginning
		Music_Rewind();
}