#pragma once
#include "raylib.h"
#pragma once
#include "grain.h"

void DrawWaveViewStatic(Wave wave, Rectangle bounds, int resolution);
void DrawGrains(GrainController gc, Rectangle bounds);
void DrawVolumeControl(Rectangle bounds, VolumePointsHandler vph);

void CheckVolumeChanges(Rectangle bounds, VolumePointsHandler *vph);