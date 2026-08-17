/*
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

by Jeffery Myers is marked with CC0 1.0. To view a copy of this license, visit https://creativecommons.org/publicdomain/zero/1.0/

*/

#include "raylib.h"
#include "math.h"

#include "resource_dir.h"	// utility header for SearchAndSetResourceDir

#define WIDTH 800
#define HEIGHT 600

#define MAXs32 2147483648

#define BUFFER_SIZE 4096
#define SAMPLE_RATE 44100

typedef struct{
	Rectangle bounds;
	int resolution;
	float speed;

	int loopStart;
	int loopEnd;
	float readPosition;
	int readDirection; // 1 or -1

	bool showLoopLines;
	bool isPingPong;
	int lineSelected; //0 is none, 1 is loopStart, 2 is loopEnd
}WaveViewParameters;

void DrawWaveVisualizer(Wave wave, WaveViewParameters wp);
void DrawWaveViewStatic(Wave wave, WaveViewParameters wp);
void DrawWaveViewLines(WaveViewParameters wp);

void MoveLoopLines(WaveViewParameters *wp, Vector2 mousePos);

float fMap(float a, float minA, float maxA, float newMin, float newMax);
float fLerp(float a, float b, float t);

int main ()
{
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	// Create the window and OpenGL context
	InitWindow(WIDTH, HEIGHT, "sample view");
	InitAudioDevice();

	SetAudioStreamBufferSizeDefault(BUFFER_SIZE);
	float buffer[BUFFER_SIZE];

	AudioStream stream = LoadAudioStream(SAMPLE_RATE, 32, 1);
    float pan = 0.0f;
    SetAudioStreamPan(stream, pan);
	PlayAudioStream(stream);

	// Utility function from resource_dir.h to find the resources folder and set it as the current working directory so we can load from it
	SearchAndSetResourceDir("resources");
	Wave testWave = LoadWave("audio/test.mp3");
	float *waveData = testWave.data;

	//initialization
	Rectangle wvpBounds = {.x = 0, .y = 0, .width = WIDTH, .height = HEIGHT/3};
	WaveViewParameters wvp = {.bounds = wvpBounds, .resolution = 2000, .showLoopLines = true, .speed = 1.0f, .isPingPong = true, .readDirection = 1};
	wvp.loopStart = wvpBounds.x + wvpBounds.width / 3;
	wvp.loopEnd = wvpBounds.x + wvpBounds.width * 2 / 3;

	wvp.readPosition = pointToSampleWP(wvp.loopStart, wvp, testWave);
	// game loop
	while (!WindowShouldClose())		// run the loop untill the user presses ESCAPE or presses the Close button on the window
	{
		Vector2 mousePosition = GetMousePosition();
		MoveLoopLines(&wvp, mousePosition);
		
		if(IsKeyPressed(KEY_DOWN)){
			wvp.speed -= 0.1f;
		}
		if(IsKeyPressed(KEY_UP)){
			wvp.speed += 0.1f;
		}
		// drawing
		BeginDrawing();
		// Setup the back buffer for drawing (clear color and depth buffers)
		ClearBackground(LIGHTGRAY);
		DrawWaveVisualizer(testWave, wvp);

		//debug text
		char debugText[20] = "Line Selected: 0";
		debugText[15] = wvp.lineSelected + '0';
		DrawText(debugText, 550, 20, 20, BLACK);

		int samplePointStart = pointToSampleWP(wvp.loopStart, wvp, testWave);
		int samplePointEnd = pointToSampleWP(wvp.loopEnd, wvp, testWave);
		sprintf(debugText, "%d", wvp.readDirection);
		DrawText(debugText, 550, 40, 20, BLACK);

		// end the frame and get ready for the next one  (display frame, poll input, etc...)
		EndDrawing();

		//audio processing
		if(IsAudioStreamProcessed(stream)){
			if(wvp.isPingPong){
				for(int i = 0; i < BUFFER_SIZE; i++){
					if(wvp.readPosition >= samplePointEnd && wvp.readDirection == 1)
						wvp.readDirection = -1;
					else if(wvp.readPosition <= samplePointStart && wvp.readDirection == -1)
						wvp.readDirection = 1;

					buffer[i] = waveData[(int)wvp.readPosition] * 0;
					wvp.readPosition += wvp.speed * 2 * wvp.readDirection;
				}
			}
			else{
				for(int i = 0; i < BUFFER_SIZE; i++){
					if(wvp.readPosition >= samplePointEnd)
						wvp.readPosition = samplePointStart;
					buffer[i] = waveData[(int)wvp.readPosition];
					wvp.readPosition += wvp.speed * 2;
				}
			}
			UpdateAudioStream(stream, buffer, BUFFER_SIZE);
		}
	}

	// cleanup
	UnloadWave(testWave);

	// destroy the window and cleanup the OpenGL context
	CloseWindow();
	return 0;
}

void MoveLoopLines(WaveViewParameters *wp, Vector2 mousePos){
	//return if loop lines are disabled, mouse is outside viewer window or mouse isnt held
	if(!wp->showLoopLines || !IsMouseButtonDown(MOUSE_LEFT_BUTTON) ||
	(mousePos.x < wp->bounds.x || mousePos.x >  wp->bounds.x + wp->bounds.width) ||
	(mousePos.y < wp->bounds.y || mousePos.y > wp->bounds.y + wp->bounds.height)){
		wp->lineSelected = 0;
		return;
	}

	if(wp->lineSelected == 0){
		float distStart = fabs(mousePos.x - wp->loopStart);
		float distEnd = fabs(mousePos.x - wp->loopEnd);

		float dist = distStart < distEnd ? distStart : distEnd;
		float selected = distStart < distEnd ? 1 : 2;
		if(dist <= 2) wp->lineSelected = selected;
	}
	
	if(wp->lineSelected == 1 && mousePos.x < wp->loopEnd - 2) wp->loopStart = mousePos.x;
	else if(wp->lineSelected == 2 && mousePos.x > wp->loopStart + 2) wp->loopEnd = mousePos.x;

}

void DrawWaveVisualizer(Wave wave, WaveViewParameters wp){
	DrawWaveViewStatic(wave, wp);
	DrawWaveViewLines(wp);
}

/// @brief Draws main viewer window and audio data
void DrawWaveViewStatic(Wave wave, WaveViewParameters wp){
	DrawRectangleLinesEx(wp.bounds, 1, BLACK);
	
	int sampleSize = wave.channels * wave.frameCount;
	float *waveData = wave.data;

	int yMid = wp.bounds.y + wp.bounds.height / 2;

	for(int i = 0; i < wp.resolution; i++){
		int x = wp.bounds.x + wp.bounds.width * i / wp.resolution;

		float yOffsetRaw = waveData[i * sampleSize / wp.resolution];
		int y = (int)fMap(yOffsetRaw, -1 , 1, wp.bounds.y + wp.bounds.height, wp.bounds.y);
		
		if(fabs(yMid - y) > 2) DrawLine(x, yMid, x, y, RED);
		else DrawPixel(x, yMid, RED);
	}
}

void DrawWaveViewLines(WaveViewParameters wp){
	if(wp.showLoopLines){
		DrawLine(wp.loopStart, wp.bounds.y, wp.loopStart, wp.bounds.y + wp.bounds.height, DARKBLUE);
		DrawLine(wp.loopEnd, wp.bounds.y, wp.loopEnd, wp.bounds.y + wp.bounds.height, DARKBLUE);
	}
}

/// @brief Maps float a, with maximum and minimum values maxA and minA, to the new maximum and minumum
/// @return Remapped value of a
float fMap(float a, float minA, float maxA, float newMin, float newMax){
	//define minA as 0 : a - minA
	//define maxA as 1 (scale range to have size 1) : (a - minA) / (maxA - minA)
	//lerp between newMin and newMax
	float t = (a - minA) / (maxA - minA);
	return fLerp(newMin, newMax, t);
}

/// @brief Float linear interpolation function
/// @return a when t = 0, b when t = 1, and interpolates for values of t between 0 and 1
float fLerp(float a, float b, float t){
	t = (t < 0) ? 0 : t; //map negatives to 0
	t = (t > 1) ? 1 : t; //map values greater than 1 to 1
	return a + (b - a) * t;
}

/// @brief Given an x coordinate, returns the sample associated to that position in the wave viewer
int pointToSampleWP(int x, WaveViewParameters wp, Wave wave){
	if(x < wp.bounds.x || x > wp.bounds.x + wp.bounds.width)
		return 0;
	int sampleSize = wave.channels * wave.frameCount;
	int final = (int)fMap(x, wp.bounds.x, wp.bounds.x + wp.bounds.width, 0, sampleSize);
	return final;
}
