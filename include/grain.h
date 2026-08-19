#pragma once
#include "raylib.h"

/// @brief One grain of the granular sampler
typedef struct{
	float readPosition; //current FRAME, not sample
	int readDirection;
	float volume; //1.0f is max
	float speed;
}Grain;

typedef struct{
	int n; //number of grains
	Grain *grains;

	int frameCount; //required to draw and to check end of sample
}GrainController;

GrainController CreateGrainController(int n, int waveSize, int randomness);
void FreeGrainController(GrainController *gc);

void ProcessAudioGrains(GrainController gc, Wave wave, float *buffer, int bufferSize);