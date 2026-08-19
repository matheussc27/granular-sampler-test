#pragma once
#include "raylib.h"
#pragma once
#include "grain.h"

typedef struct{
    float volume;
    float offset;

    bool selected;
}VolumeControlPoint;

void DrawWaveViewStatic(Wave wave, Rectangle bounds, int resolution);
void DrawGrains(GrainController gc, Rectangle bounds);
void DrawVolumeControl(Rectangle bounds, VolumeControlPoint *vcps, int n, int maxOffset);

VolumeControlPoint *CreateVolumePoints(int n, int maxOffset);
void FreeVolumePoints(VolumeControlPoint *vcps);