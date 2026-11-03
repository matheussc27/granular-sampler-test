#include "gui.h"

#include "helpers.h"
#include "math.h"

#include "stdlib.h"
#include "stdio.h"


/// @brief Draws main viewer window and audio data
void DrawWaveViewStatic(Wave wave, Rectangle bounds, int resolution){
	
	int sampleSize = wave.channels * wave.frameCount;
	float *waveData = wave.data;

	int yMid = bounds.y + bounds.height / 2;

	for(int i = 0; i < resolution; i++){
		int x = bounds.x + bounds.width * i / resolution;

		float yOffsetRaw = waveData[i * (sampleSize / resolution)];
		int y = (int)fMap(yOffsetRaw, -1 , 1, bounds.y + bounds.height, bounds.y);
		
		if(fabs(yMid - y) > 2) DrawLine(x, yMid, x, y, RED);
		else DrawPixel(x, yMid, RED);
	}

	DrawRectangleLinesEx(bounds, 1, BLACK);
}

void DrawGrains(GrainController gc, Rectangle bounds){
	float margin = 10;
	float grainSeparation = (gc.n > 1) ? (bounds.height - margin * 2) / (gc.n - 1) : 0;
	for(int i = 0; i < gc.n; i++){
		int y = bounds.y + margin + grainSeparation * i;
		int x = (int)fMap(gc.grains[i].readPosition, 0, gc.frameCount, bounds.x, bounds.x + bounds.width);
		DrawCircle(x, y, 1, BLACK);
	}

	DrawRectangleLinesEx(bounds, 1, BLACK);
}

void DrawVolumeControl(Rectangle bounds, VolumePointsHandler vph){
	int x = 0, y = 0;
	int prevX = 0, prevY = 0;

	for(int i = 0; i < vph.n; i++){
		VolumeControlPoint vp = vph.vcps[i];

		x = (int)fMap(vp.offset, 0, vph.maxOffset, bounds.x, bounds.x + bounds.width);
		y = (int)fMap(vp.volume, 0, 1, bounds.y + bounds.height, bounds.y);
		
		if(i > 0)
			DrawLine(x, y, prevX, prevY, BLACK);

		prevX = x;
		prevY = y;
	}

	for(int i = 1; i < vph.n - 1; i++){
		VolumeControlPoint vp = vph.vcps[i];
		x = (int)fMap(vp.offset, 0, vph.maxOffset, bounds.x, bounds.x + bounds.width);
		y = (int)fMap(vp.volume, 0, 1, bounds.y + bounds.height, bounds.y);
		DrawCircle(x, y, 3, BLACK);
	}

	DrawRectangleLinesEx(bounds, 1, BLACK);
}

void VolumeGUIHandler(){
	//
}