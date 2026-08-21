
#include "raylib.h"
#include "math.h"
#include "stdlib.h"

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
	Wave testWave = LoadWave("audio/gymnopedie1.mp3");
	float *waveData = testWave.data;

	//initialization
	Rectangle waveViewBounds = {.x = 10, .y = 10, .width = WIDTH/4 * 3, .height = HEIGHT/4};
	Rectangle grainViewBounds = {.x = 10, .y = HEIGHT/4 + 20, .width = WIDTH/4 * 3, .height = HEIGHT/4 + 10};
	Rectangle volumeBounds = {.x = 10, .y = HEIGHT/2 + 40, .width = WIDTH/4 * 3, .height = HEIGHT - (HEIGHT/2 + 60)};
	
	int randomness = 100;
	GrainController gc = CreateGrainController(10, testWave.channels * testWave.frameCount, randomness);
	
	int nVcps = 5;
	VolumePointsHandler vph = CreateVolumeHandler(5, randomness);
	// game loop
	while (!WindowShouldClose())		// run the loop untill the user presses ESCAPE or presses the Close button on the window
	{
		
		CheckVolumeChanges(volumeBounds, &vph);
		UpdateGrainVolume(gc, vph);

		// drawing
		BeginDrawing();
		// Setup the back buffer for drawing (clear color and depth buffers)
		ClearBackground(LIGHTGRAY);
		DrawWaveViewStatic(testWave, waveViewBounds, 2000);
		DrawGrains(gc, grainViewBounds);
		DrawVolumeControl(volumeBounds, vph);

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
	FreeVolumeHandler(vph);

	// destroy the window and cleanup the OpenGL context
	CloseWindow();
	return 0;
}
