/*
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

by Jeffery Myers is marked with CC0 1.0. To view a copy of this license, visit https://creativecommons.org/publicdomain/zero/1.0/

*/

#include "raylib.h"
#include "math.h"
#include "stdlib.h"

#include "resource_dir.h"	// utility header for SearchAndSetResourceDir
#include "helpers.h"

#define WIDTH 1000
#define HEIGHT 680

#define BUFFER_SIZE 4096
#define SAMPLE_RATE 44100

/// @brief One grain of the granular sampler
typedef struct{
	float readPosition;
	int readDirection;
	float volume; //1.0f is max
	float speed;
}Grain;

typedef struct{
	int n; //number of grains
	Grain *grains;

	int waveSize; //required to draw and to check end of sample
}GrainController;


void DrawWaveViewStatic(Wave wave, Rectangle bounds, int resolution);
void DrawGrains(GrainController gc, Rectangle bounds);
GrainController CreateGrainController(int n, int waveSize, int randomness);
void FreeGrainController(GrainController *gc);

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
	Wave testWave = LoadWave("audio/gymnopedie1.mp3");
	float *waveData = testWave.data;

	//initialization
	Rectangle waveViewBounds = {.x = 0, .y = 0, .width = WIDTH/4 * 3, .height = HEIGHT/3};
	Rectangle grainViewBounds = {.x = 0, .y = HEIGHT/3 + 10, .width = WIDTH/4 * 3, .height = HEIGHT - (HEIGHT/3 + 10)};
	GrainController gc = CreateGrainController(20, testWave.channels * testWave.frameCount, 10);
	// game loop
	while (!WindowShouldClose())		// run the loop untill the user presses ESCAPE or presses the Close button on the window
	{
		Vector2 mousePosition = GetMousePosition();
		
		// drawing
		BeginDrawing();
		// Setup the back buffer for drawing (clear color and depth buffers)
		ClearBackground(LIGHTGRAY);
		DrawWaveViewStatic(testWave, waveViewBounds, 1000);
		DrawGrains(gc, grainViewBounds);

		// end the frame and get ready for the next one  (display frame, poll input, etc...)
		EndDrawing();

		if(IsAudioStreamProcessed(stream)){
			ProcessAudioGrains(gc, testWave, buffer);
			UpdateAudioStream(stream, buffer, BUFFER_SIZE);
		}
	}

	// cleanup
	UnloadWave(testWave);
	FreeGrainController(&gc);

	// destroy the window and cleanup the OpenGL context
	CloseWindow();
	return 0;
}

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
		int x = (int)fMap(gc.grains[i].readPosition, 0, gc.waveSize, bounds.x, bounds.x + bounds.width);
		DrawCircle(x, y, 1, BLACK);
	}
}

/// @param randomness value between 0 and 100, changes offset in relation to main read point (0 is no offset)
GrainController CreateGrainController(int n, int waveSize, int randomness){
	GrainController gc;
	gc.grains = malloc(sizeof(Grain) * n);
	gc.n = n;
	gc.waveSize = waveSize;

	int start = waveSize/6;
	float offsetMultiplier = (float)randomness / 1000.0f;

	for(int i = 0; i < n; i++){
		gc.grains[i].readDirection = 1;
		gc.grains[i].speed = 1;
		gc.grains[i].volume = 1/(float)n; //normalization
		gc.grains[i].readPosition = start + GetRandomValue(-waveSize * offsetMultiplier, waveSize * offsetMultiplier);
	}
	return gc;
}

void FreeGrainController(GrainController *gc){
	free(gc->grains);
}

/// @brief Fills the buffer with new data considering the grains and updates grain data. Called when stream finishes processing.
//optimization idea: get smallest start and higher end and loop only once checking grains on each point
void ProcessAudioGrains(GrainController gc, Wave wave, float *buffer){
	int waveSize = wave.channels * wave.frameCount;
	float *waveData = wave.data;

	//reset buffer
	for(int i = 0; i < BUFFER_SIZE; i++)
		buffer[i] = 0;

	//fill buffer
	for(int i = 0; i < gc.n; i++){
		for(int j = 0; j < BUFFER_SIZE; j++){
			Grain g = gc.grains[i];
			buffer[j] += waveData[(int)g.readPosition] * g.volume; //fix with proper stereo buffer filling
			gc.grains[i].readPosition = g.readPosition + g.speed * 2;
			
			if(gc.grains[i].readPosition >= waveSize)
				gc.grains[i].readPosition = 0;
		}
	}
}
