#pragma once
#include "raylib.h"
#pragma once
#include "grain.h"

typedef struct{
    float volume;
    float offset;

}VolumeControlPoint;

typedef struct{
    VolumeControlPoint *vcps;
    int n;

    int maxOffset;
    VolumeControlPoint *selected;
}VolumePointsHandler;

void DrawWaveViewStatic(Wave wave, Rectangle bounds, int resolution);
void DrawGrains(GrainController gc, Rectangle bounds);
void DrawVolumeControl(Rectangle bounds, VolumePointsHandler vph);

VolumePointsHandler CreateVolumeHandler(int n, int maxOffset);
void FreeVolumeHandler(VolumePointsHandler vph);

void CheckVolumeChanges(Rectangle bounds, VolumePointsHandler *vph);