#pragma once
#include "raylib.h"

/// @brief One grain of the granular sampler
typedef struct{
	float readPosition; //current FRAME, not sample
	int readDirection;
	float volume; //1.0f is max
	float speed;
    float offset; //value between -100 and 100
}Grain;

typedef struct{
	int n; //number of grains
	Grain *grains; //grains[0] is reference

	int frameCount; //required to draw and to check end of sample
}GrainController;

typedef struct{
    float volume;
    float offset;

}VolumeControlPoint;

typedef struct{
    VolumeControlPoint *vcps;
    int n;

    int maxOffset;
    VolumeControlPoint *selected;
}VolumePointsHandler;

GrainController CreateGrainController(int n, int waveSize, int randomness);
void FreeGrainController(GrainController *gc);

VolumePointsHandler CreateVolumeHandler(int n, int maxOffset);
void FreeVolumeHandler(VolumePointsHandler vph);

void ProcessAudioGrains(GrainController gc, Wave wave, float *buffer, int bufferSize);
void UpdateGrainVolume(GrainController gc, VolumePointsHandler vph);