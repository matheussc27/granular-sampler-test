#pragma once
#include "raylib.h"

//value to scale after normalization
#define VOLUME_SCALE 3

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
    bool isFixed;

}VolumeControlPoint;

typedef struct{
    VolumeControlPoint vcps[100];
    int n;

    int maxOffset;
    int selectedIndex;
}VolumePointsHandler;

GrainController CreateGrainController(int n, int frameCount, int randomness);
void FreeGrainController(GrainController *gc);

VolumePointsHandler CreateVolumeHandler(int maxOffset);
int InsertVolumePoint(VolumePointsHandler *vph, VolumeControlPoint vp);

void ProcessAudioGrains(GrainController gc, Wave wave, float *buffer, int bufferSize);
void UpdateGrainVolume(GrainController *gc, VolumePointsHandler vph);