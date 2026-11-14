#pragma once
#include "raylib.h"
#pragma once
#include "grain.h"

#define VP_RADIUS 3 //Radius for drawing volume points (and for checking for clicks).

/**
 * @brief Draws the wave data as vertical lines, and a border around the area.
 * @param bounds Area used for drawing.
 * @param resolution Number of lines used for drawing.
 */
void DrawWaveViewStatic(Wave wave, Rectangle bounds, int resolution);

void DrawWaveViewReadLine(Grain mainGrain, Rectangle bounds, int frameCount);

/**
 * @brief Draws the grains as individual circles.
 * @param bounds Area used for drawing.
 */
void DrawGrains(GrainController gc, Rectangle bounds);
/**
 * @brief Draws the volume control points as circles connected by lines.
 * @param bounds Area used for drawing.
 */
void DrawVolumeControl(VolumePointsHandler vph, Rectangle bounds);

/**
 * @brief Handles creating and moving volume points with the mouse.
 * @param vph Volume Handler passed as pointer since the volume points can be changed.
 * @param bounds Area used for checking if mouse position is valid.
 * @return true if a volume point has been updated (for now, only after update has been completed, that is, after a point has been deselected).
 */
bool VolumeGUIHandler(VolumePointsHandler *vph, Rectangle bounds, Vector2 mousePos);

/**
 * @brief Given an absolute screen position and a limiting area, calculates the offset and volume associated to that position. 
 * 
 * Inverse of `getVPscreenPos()`
 * 
 * @param pos Absolute screen position. The x value is used for the offset, and the y value is used for the volume.
 * @param bounds Limiting area of the conversion.
 * @param maxOffset Maximum offset a volume point can have, necessary for properly converting the x position to the offset.
 */
static VolumeControlPoint getVPFromPos(Vector2 pos, Rectangle bounds, float maxOffset);

/**
 * @brief Given a volume point and a limiting area, calcules the absolute screen position of that point.
 * 
 * Inverse of `getVPFromPos()`
 * 
 * @param vp Volume point. The offset is converted to an x value, and the volume is converted to a y value
 * @param bounds Limiting area of the conversion.
 * @param maxOffset Maximum offset a volume point can have, necessary for properly converting the offset to the x position.
 */
static Vector2 getVPscreenPos(VolumeControlPoint vp, Rectangle bounds, float maxOffset);