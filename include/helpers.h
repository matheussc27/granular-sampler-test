#pragma once
#include "math.h"

#define s32MAX 2147483648

/// @brief Maps float a, with maximum and minimum values maxA and minA, to the new maximum and minumum
/// @return Remapped value of a
float fMap(float a, float minA, float maxA, float newMin, float newMax);

/// @brief Float linear interpolation function
/// @return a when t = 0, b when t = 1, and interpolates for values of t between 0 and 1
float fLerp(float a, float b, float t);