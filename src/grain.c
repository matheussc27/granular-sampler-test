#include "grain.h"
#include "stdlib.h"
#include "helpers.h"

#include <stdio.h>
#include <math.h>

/// @param randomness value between 0 and 100, changes offset in relation to main read point (0 is no offset)
GrainController CreateGrainController(int n, int frameCount, int randomness){
	GrainController gc;
	gc.grains = malloc(sizeof(Grain) * n);
	gc.n = n;
	gc.frameCount = frameCount;

	float start = 0;

	for(int i = 0; i < n; i++){
    Grain *g = &gc.grains[i];

		g->readDirection = 1;
		g->speed = 1;
		g->volume = 1;
    g->offset = i >= 1 ? GetRandomValue(-randomness/2 * 10, randomness/2 * 10)/10.0f : 0;
		g->readPosition = (int)(start + g->offset * ((float)frameCount / 100.0f) + frameCount) % frameCount;
		printf("%.2f", g->offset);
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

VolumePointsHandler CreateVolumeHandler(int maxOffset){
	VolumePointsHandler vph;

	vph.maxOffset = maxOffset;
	VolumeControlPoint start = {.offset = 0, .volume = 1, .isFixed = true};
	VolumeControlPoint end = {.offset = maxOffset, .volume = 0, .isFixed = true};

	vph.vcps[0] = start;
	vph.vcps[1] = end;
	vph.n = 2;

	vph.selectedIndex = -1;

	return vph;
}

void InsertVolumePoint(VolumePointsHandler *vph, VolumeControlPoint vp){
	int insertIndex = -1;

	for(int i = vph->n - 1; i >= 0; i--){
		VolumeControlPoint curr = vph->vcps[i];
		if(curr.offset < vp.offset){
			insertIndex = i + 1;
			break;
		}

		if(insertIndex == -1){
			vph->vcps[i+1] = vph->vcps[i];
		}
	}
	vph->vcps[insertIndex] = vp;
	vph->n++;
}

void UpdateGrainVolume(GrainController *gc, VolumePointsHandler vph){
	float total;

	for(int i = 0; i < gc->n; i++){
		Grain *g = &gc->grains[i];
		int index = -1;

		for(int j = 0; j < vph.n; j++){
			VolumeControlPoint vp = vph.vcps[j];
			if(abs(g->offset) <= vp.offset){
				index = j;
				break;
			}
		}
		VolumeControlPoint prev = vph.vcps[index - 1];
		VolumeControlPoint next = vph.vcps[index];

		g->volume = fMap(g->offset, prev.offset, next.offset, prev.volume, next.volume);
		total += g->volume;
	}

	//normalizing
	for(int i = 0; i < gc->n; i++){
		Grain *g = &gc->grains[i];
		g->volume /= sqrt(total);
	}
}