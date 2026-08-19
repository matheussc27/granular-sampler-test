#include "gui.h"
#include "helpers.h"

/// @brief Draws main viewer window and audio data
void DrawWaveViewStatic(Wave wave, Rectangle bounds, int resolution){
	DrawRectangleLinesEx(bounds, 1, BLACK);
	
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
}

void DrawGrains(GrainController gc, Rectangle bounds){
	DrawRectangleLinesEx(bounds, 1, BLACK);
	float margin = 10;
	float grainSeparation = (gc.n > 1) ? (bounds.height - margin * 2) / (gc.n - 1) : 0;
	for(int i = 0; i < gc.n; i++){
		int y = bounds.y + margin + grainSeparation * i;
		int x = (int)fMap(gc.grains[i].readPosition, 0, gc.frameCount, bounds.x, bounds.x + bounds.width);
		DrawCircle(x, y, 1, BLACK);
	}
}

void DrawVolumeControl(Rectangle bounds){

}