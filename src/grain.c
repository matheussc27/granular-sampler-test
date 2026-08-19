#include "grain.h"

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
        g->offset = GetRandomValue(-randomness, randomness);
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
			buffer[2 * j] += waveData[samplePos] * g->volume; //fix with proper stereo buffer filling
			buffer[2 * j + 1] += waveData[samplePos + 1] * g->volume;
			g->readPosition += g->speed;
			
			if(g->readPosition >= wave.frameCount)
				g->readPosition = 0;
		}
	}
}