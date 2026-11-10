
#include "raylib.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "resource_dir.h"	// utility header for SearchAndSetResourceDir
#include "helpers.h"
#include "grain.h"
#include "gui.h"

#define WIDTH 1000
#define HEIGHT 680

#define BUFFER_SIZE 2048 //in FRAMES (not samples)
#define SAMPLE_RATE 48000

int main ()
{
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	// Create the window and OpenGL context
	InitWindow(WIDTH, HEIGHT, "sample view");
	InitAudioDevice();

	SetAudioStreamBufferSizeDefault(BUFFER_SIZE);
	float buffer[BUFFER_SIZE * 2];

	AudioStream stream = LoadAudioStream(SAMPLE_RATE, 32, 2);
  float pan = 0.5f;
  SetAudioStreamPan(stream, pan);
	PlayAudioStream(stream);

	// Utility function from resource_dir.h to find the resources folder and set it as the current working directory so we can load from it
	SearchAndSetResourceDir("resources");
	Wave testWave = LoadWave("audio/gymnopedie1.wav");
	WaveFormat(&testWave, SAMPLE_RATE, 32, 2);

	//initialization
	int marginLeft = 10;
	int guiWidth = WIDTH/6 * 3;
	Rectangle waveViewBounds = {.x = marginLeft, .y = 10, .width = guiWidth, .height = HEIGHT/4};
	Rectangle grainViewBounds = {.x = marginLeft, .y = HEIGHT/4 + 20, .width = guiWidth, .height = HEIGHT/4 + 10};
	Rectangle volumeBounds = {.x = marginLeft, .y = HEIGHT/2 + 40, .width = guiWidth, .height = HEIGHT/4 + 10};
	
	float randomness = 50;
	GrainController gc = CreateGrainController(20, testWave.frameCount, randomness);
	VolumePointsHandler vph = CreateVolumeHandler(randomness);

	InsertVolumePoint(&vph, (VolumeControlPoint){0.15f, randomness * 0.5f, false});
	UpdateGrainVolume(&gc, vph);

	// game loop
	while (!WindowShouldClose())		// run the loop untill the user presses ESCAPE or presses the Close button on the window
	{
		Vector2 mousePos = GetMousePosition();
		if(VolumeGUIHandler(&vph, volumeBounds, mousePos)){
			UpdateGrainVolume(&gc, vph);
		}


		// drawing
		BeginDrawing();
		// Setup the back buffer for drawing (clear color and depth buffers)
		ClearBackground(LIGHTGRAY);
		DrawWaveViewStatic(testWave, waveViewBounds, 600);
		DrawGrains(gc, grainViewBounds);
		DrawVolumeControl(vph, volumeBounds);

		DrawText(TextFormat("Grain Count: %d", gc.n), WIDTH/2 + marginLeft * 2, 10, 20, BLACK);
		DrawText(TextFormat("Randomness: %.1f", randomness), WIDTH/2 + marginLeft * 2, 60, 20, BLACK);

		// end the frame and get ready for the next one  (display frame, poll input, etc...)
		EndDrawing();

		if(IsAudioStreamProcessed(stream)){
			ProcessAudioGrains(gc, testWave, buffer, BUFFER_SIZE);

			//before passing buffer to audio stream, clip all samples at [-1.0f, 1.0f] (safeguarding)
			for(int i = 0; i < BUFFER_SIZE * 2; i++)
				buffer[i] = fLimit(buffer[i], -1.0f, 1.0f);
				
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
