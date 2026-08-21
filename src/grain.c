#include "grain.h"
#include "stdlib.h"
#include "helpers.h"
#include "math.h"

/// @param randomness value between 0 and 100, changes offset in relation to main read point (0 is no offset)
GrainController CreateGrainController(int n, int frameCount, int randomness){
	GrainController gc;
	gc.grains = malloc(sizeof(Grain) * n);
	gc.n = n;
	gc.frameCount = frameCount;

	float start = (float)frameCount / 8.0f;

	for(int i = 0; i < n; i++){
        Grain *g = &gc.grains[i];

		g->readDirection = 1;
		g->speed = 1;
		g->volume = 1/(float)n; //normalization
        g->offset = i > 1 ? GetRandomValue(-randomness, randomness) : 0;
		g->readPosition = start + g->offset * ((float)frameCount / 1000.0f);
	}
	return gc;
}

void FreeGrainController(GrainController *gc){
	free(gc->grains);
}

/// @brief Fills the buffer with new data considering the grains and updates grain data. Called when stream finishes processing.
//optimization idea: get smallest start and higher end and loop only once checking grains on each point
void ProcessAudioGrains(GrainController gc, Wave wave, float *buffer, int bufferSize){
	float *waveData = wave.data;

	//reset buffer
	for(int i = 0; i < bufferSize * 2; i++)
		buffer[i] = 0;

	//fill buffer
	for(int i = 0; i < gc.n; i++){
		Grain *g = &gc.grains[i];
		for(int j = 0; j < bufferSize; j++){
			int samplePos = g->readPosition * 2;
			buffer[2*j] += waveData[samplePos] * g->volume;
			buffer[2*j + 1] += waveData[samplePos + 1] * g->volume;
			g->readPosition += g->speed;
			
			if(g->readPosition >= wave.frameCount)
				g->readPosition = 0;
		}
	}
}

void UpdateGrainVolume(GrainController gc, VolumePointsHandler vph){
	float totalVolume = 0; //for normalizing

	for(int i = 0; i < gc.n; i++){
		Grain *g = &gc.grains[i];
		VolumeControlPoint vp1, vp2;

		//get what volume points the grain is between
		for(int j = 1; j < vph.n; j++){
			if(g->offset > vph.vcps[i - 1].offset && g->offset < vph.vcps[i].offset){
				vp1 = vph.vcps[i - 1];
				vp2 = vph.vcps[i];
				break;
			}
		}
		//get volume from lerp of volume points
		float newVolume = fMap(g->offset, vp1.offset, vp2.offset, vp1.volume, vp2.volume);
		g->volume = newVolume; //not normalized, fixed later
		totalVolume += newVolume;
	}

	//normalize volumes
	for(int i = 0; i < gc.n; i++){
		Grain *g = &gc.grains[i];
		g->volume = g->volume / totalVolume;
	}
}

VolumePointsHandler CreateVolumeHandler(int n, int maxOffset){
    VolumePointsHandler vph;

    vph.vcps = malloc(sizeof(VolumeControlPoint) * n);
    vph.n = n;
    vph.maxOffset = maxOffset;
    vph.selected = NULL;

    for(int i = 0; i < n; i++){
        VolumeControlPoint *vp = &vph.vcps[i];
        vp->offset = fMap(i, 0, n - 1, -maxOffset, maxOffset);
        vp->volume = 0.5f;
    }
    return vph;
}

void FreeVolumeHandler(VolumePointsHandler vph){
    free(vph.vcps);
}