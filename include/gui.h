#pragma once
#include "raylib.h"
#pragma once
#include "grain.h"

void DrawWaveViewStatic(Wave wave, Rectangle bounds, int resolution);
void DrawGrains(GrainController gc, Rectangle bounds);
void DrawVolumeControl(VolumePointsHandler vph, Rectangle bounds);

bool VolumeGUIHandler(VolumePointsHandler *vph, Rectangle bounds, Vector2 mousePos);

VolumeControlPoint getVPFromPos(Vector2 pos, Rectangle bounds, float maxOffset);
Vector2 getVPscreenPos(VolumeControlPoint vp, Rectangle bounds, float maxOffset);