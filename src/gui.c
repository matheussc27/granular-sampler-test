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

void DrawVolumeControl(Rectangle bounds, VolumePointsHandler vph){
    DrawRectangleLinesEx(bounds, 1, BLACK);
    char text[30];
    Vector2 prev;
    Vector2 boundLim = {bounds.x + bounds.width, bounds.y + bounds.height};

    //line and circle renderpasses separate to avoid lines drawing over circles 
    for(int i = 0; i < vph.n; i++){
        VolumeControlPoint *vp = &vph.vcps[i];
        int x = (int)fMap(vp->offset, -vph.maxOffset, vph.maxOffset, bounds.x, boundLim.x);
        int y = (int)fMap(vp->volume, 0, 1, boundLim.y, bounds.y);

        if(i >= 1) DrawLine(x, y, prev.x, prev.y, BLACK);

        prev.x = x; 
        prev.y = y;
    }

    for(int i = 0; i < vph.n; i++){
        VolumeControlPoint *vp = &vph.vcps[i];
        int x = (int)fMap(vp->offset, -vph.maxOffset, vph.maxOffset, bounds.x, boundLim.x);
        int y = (int)fMap(vp->volume, 0, 1, boundLim.y, bounds.y);

        if(vp != vph.selected)
            DrawCircle(x, y, 4, RED);
        else
            DrawCircle(x, y, 4, BLUE);
        sprintf(text, "%.2f", vp->volume);
        DrawText(text, x - 12, y + -30, 12, BLACK);
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

void CheckVolumeChanges(Rectangle bounds, VolumePointsHandler *vph){
    Vector2 boundLim = {.x = bounds.x + bounds.width, .y = bounds.y + bounds.height};

    Vector2 mousePos = GetMousePosition();

    if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
        //outside gui window
        if(mousePos.x < bounds.x || mousePos.x > boundLim.x
        || mousePos.y < bounds.y || mousePos.y > boundLim.y){
            return;
        }

        for(int i = 0; i < vph->n; i++){
            VolumeControlPoint *vp = &vph->vcps[i];
            int x = (int)fMap(vp->offset, -vph->maxOffset, vph->maxOffset, bounds.x, boundLim.x);
            int y = (int)fMap(vp->volume, 0, 1, boundLim.y, bounds.y);

            if(pow(mousePos.x - x, 2) + pow(mousePos.y - y, 2) < 5*5 && vph->selected == NULL){
                vph->selected = vp;
                break;
            }
        }
    }
    else
        vph->selected = NULL;

    if(vph->selected != NULL)
        vph->selected->volume = fMap(mousePos.y, boundLim.y, bounds.y, 0, 1);
}