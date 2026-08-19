#include "gui.h"
#include "helpers.h"
#include "stdlib.h"
#include "stdio.h"

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

void DrawVolumeControl(Rectangle bounds, VolumeControlPoint *vcps, int n, int maxOffset){
    DrawRectangleLinesEx(bounds, 1, BLACK);
    char text[30];
    Vector2 prev;
    Vector2 boundLim = {bounds.x + bounds.width, bounds.y + bounds.height};

    //line and circle renderpasses separate to avoid lines drawing over circles 
    for(int i = 0; i < n; i++){
        int x = (int)fMap(vcps[i].offset, -maxOffset, maxOffset, bounds.x, boundLim.x);
        int y = (int)fMap(vcps[i].volume, 0, 1, bounds.y, boundLim.y);

        if(i >= 1) DrawLine(x, y, prev.x, prev.y, BLACK);

        prev.x = x; 
        prev.y = y;
    }

    for(int i = 0; i < n; i++){
        int x = (int)fMap(vcps[i].offset, -maxOffset, maxOffset, bounds.x, boundLim.x);
        int y = (int)fMap(vcps[i].volume, 0, 1, bounds.y, boundLim.y);

        DrawCircle(x, y, 4, RED);
        sprintf(text, "%.2f", vcps[i].volume);
        DrawText(text, x - 12, y + -30, 12, BLACK);
    }
}

VolumeControlPoint *CreateVolumePoints(int n, int maxOffset){
    VolumeControlPoint *vcps = malloc(sizeof(VolumeControlPoint) * n);

    for(int i = 0; i < n; i++){
        vcps[i].offset = fMap(i, 0, n - 1, -maxOffset, maxOffset);
        vcps[i].volume = 0.5f;
        vcps[i].selected = false;
    }
    return vcps;
}

void FreeVolumePoints(VolumeControlPoint *vcps){
    free(vcps);
}