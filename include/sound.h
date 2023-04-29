#pragma once
#include <SDL2/SDL.h>

typedef struct {
    Uint8* pos;
    Uint32 len;
    Uint8  vol; // volume, 0-128
} SOUND_track;

// plays a track if there's a space for one. if not, returns -1; 0 on success
int SOUND_Play(SOUND_track track);

// sets up an audio device using the spec. returns -1 on fail; 0 on success
int SOUND_OpenDevice();

// close the SOUND_device
void SOUND_CloseDevice();

// load wav and convert to current spec. The first wav to be loaded sets the 
// spec. returns 0 on success and -1 on fail
int SOUND_LoadWAV(const char* fname, SOUND_track* track);
