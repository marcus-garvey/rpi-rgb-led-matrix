#include <assert.h>
#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>

#include "thread.h"
#include "led-matrix.h"
#include "PPMImage.h"
#include "RGBMatrixManipulator.h"
#include "TestDrawers.h"
#include "DrawSimpleText.h"
#include "DrawRedAlert.h"
#include "DrawAnimation.h"
using std::min;
using std::max;


int main( int argc, const char* argv[] )
{
	int demo = 0;
	if (argc > 1) {
		demo = atoi(argv[1]);
	}
	fprintf(stderr, "Using demo %d\n", demo);

	GPIO io;
	if (!io.Init())
	return 1;

	RGBMatrix m(&io);
	RGBMatrixManipulator *image_gen = NULL; 
	switch(demo) {
	case 0:
		image_gen = new DrawSimpleText(&m);
		break;
	case 1:
		image_gen = new SimpleSquare(&m);
		break;
	case 2:
		image_gen = new DrawRedAlert(&m);
		break;
	case 3:
		image_gen = new ColorPulseGenerator(&m);
		break;
	case 4:
		image_gen = new DrawAnimation(&m);
		break;
	default:
		printf( "\nNothing todo\n\n" );
		exit(0);
	}


	RGBMatrixManipulator *updater = new DisplayUpdater(&m);
	updater->Start(10);  // high priority
	if(image_gen != NULL)
	image_gen->Start();

	// Things are set up. Just wait for <RETURN> to be pressed.
	printf("Press <RETURN> to exit and reset LEDs\n");
	getchar();

	// Stopping threads and wait for them to join.
	if(image_gen != NULL)
	delete image_gen;
	delete updater;

	// Final thing before exit: clear screen and update once, so that
	// we don't have random pixels burn
	m.ClearScreen();
	m.SwapScreen(false);
	m.UpdateScreen();

	return 0;
}
