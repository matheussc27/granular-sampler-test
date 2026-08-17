#include "helpers.h"

float fMap(float a, float minA, float maxA, float newMin, float newMax){
	//define minA as 0 : a - minA
	//define maxA as 1 (scale range to have size 1) : (a - minA) / (maxA - minA)
	//lerp between newMin and newMax
	float t = (a - minA) / (maxA - minA);
	return fLerp(newMin, newMax, t);
}
float fLerp(float a, float b, float t){
	t = (t < 0) ? 0 : t; //map negatives to 0
	t = (t > 1) ? 1 : t; //map values greater than 1 to 1
	return a + (b - a) * t;
}