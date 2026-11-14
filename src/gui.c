#include "gui.h"

#include "helpers.h"
#include "math.h"

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

void DrawWaveViewReadLine(Grain mainGrain, Rectangle bounds, int frameCount){
	int x = (int)fMap(mainGrain.readPosition, 0, frameCount, bounds.x, bounds.x + bounds.width);
	DrawLine(x, bounds.y, x, bounds.y + bounds.height, BLACK);
}

void DrawGrains(GrainController gc, Rectangle bounds){
	float margin = 10;
	float grainSeparation = (gc.n > 1) ? (bounds.height - margin * 2) / (gc.n - 1) : 0;
	for(int i = 0; i < gc.n; i++){
		int y = bounds.y + margin + grainSeparation * i;
		int x = (int)fMap(gc.grains[i].readPosition, 0, gc.frameCount, bounds.x, bounds.x + bounds.width);
		DrawCircle(x, y, 2, (Color){gc.grains[i].rawVolume*255, 0, 0, 255});
	}

	DrawRectangleLinesEx(bounds, 1, BLACK);
}

void DrawVolumeControl(VolumePointsHandler vph, Rectangle bounds){
	Vector2 curr;
	Vector2 prev;

	for(int i = 0; i < vph.n; i++){
		curr = getVPscreenPos(vph.vcps[i], bounds, vph.maxOffset);
		
		if(i > 0)
			DrawLine(curr.x, curr.y, prev.x, prev.y, BLACK);

		prev = curr;
	}

	for(int i = 1; i < vph.n - 1; i++){
		curr = getVPscreenPos(vph.vcps[i], bounds, vph.maxOffset);
		DrawCircle(curr.x, curr.y, VP_RADIUS, BLACK);
	}

	DrawRectangleLinesEx(bounds, 1, BLACK);
}


bool VolumeGUIHandler(VolumePointsHandler *vph, Rectangle bounds, Vector2 mousePos){
	bool result = false;

	//when to update
	if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT) || IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)){
		vph->selectedIndex = -1;
		result = true;
	}

	Vector2 lim = {.x = bounds.x + bounds.width, .y = bounds.y + bounds.height};

	//either select, create new or delete

	int mouseButton = 0; //0: not pressed, 1: left button, 2: right button
	if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) mouseButton = 1;
	else if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) mouseButton = 2;

	if(mouseButton != 0){

		//mouse out of bounds, leave
		if(mousePos.x < bounds.x || mousePos.x > lim.x
		|| mousePos.y < bounds.y || mousePos.y > lim.y)
				return result;
			
		//check if there is one in mouse pos
		for(int i = 0; i < vph->n; i++){
			VolumeControlPoint vp = vph->vcps[i];
			if(vp.isFixed)
				continue;

			Vector2 screenPos = getVPscreenPos(vp, bounds, vph->maxOffset);
			if(pow(screenPos.x - mousePos.x, 2) + pow(screenPos.y - mousePos.y, 2) < VP_RADIUS * VP_RADIUS * 10){
				vph->selectedIndex = i;
				break;
			}
		}

		//add new point
		if(mouseButton == 1){
			if(vph->selectedIndex == -1){
				VolumeControlPoint newPoint =  getVPFromPos(mousePos, bounds, vph->maxOffset);
				newPoint.isFixed = false;

				int index = InsertVolumePoint(vph, newPoint);
				vph->selectedIndex = index;
			}
		}
		//delete point
		if(mouseButton == 2){
			if(vph->selectedIndex != -1)
				RemoveVolumePoint(vph, vph->selectedIndex);
			vph->selectedIndex = -1;
		}
	}

	//move selected
	if(vph->selectedIndex != -1){
		VolumeControlPoint *selected = &vph->vcps[vph->selectedIndex];
		VolumeControlPoint prev = vph->vcps[vph->selectedIndex - 1], next = vph->vcps[vph->selectedIndex + 1];

		Vector2 prevPos = getVPscreenPos(prev, bounds, vph->maxOffset);
		Vector2 nextPos = getVPscreenPos(next, bounds, vph->maxOffset);

		mousePos.x = fLimit(mousePos.x, prevPos.x, nextPos.x); //limited by adjacent points
		mousePos.y = fLimit(mousePos.y, bounds.y, lim.y);

		*selected = getVPFromPos(mousePos, bounds, vph->maxOffset);
	}

	return result;
}

static Vector2 getVPscreenPos(VolumeControlPoint vp, Rectangle bounds, float maxOffset){
	Vector2 screenPos;
	Vector2 lim = {.x = bounds.x + bounds.width, .y = bounds.y + bounds.height};

	screenPos.x = fMap(vp.offset, 0, maxOffset, bounds.x, lim.x);
	screenPos.y = fMap(vp.volume, 0, 1, lim.y, bounds.y);

	return screenPos;
}

static VolumeControlPoint getVPFromPos(Vector2 pos, Rectangle bounds, float maxOffset){
	VolumeControlPoint vp;
	Vector2 lim = {.x = bounds.x + bounds.width, .y = bounds.y + bounds.height};

	vp.offset = fMap(pos.x, bounds.x, lim.x, 0, maxOffset);
	vp.volume = fMap(pos.y, lim.y, bounds.y, 0, 1);

	return vp;
}