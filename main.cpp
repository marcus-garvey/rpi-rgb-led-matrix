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
#include "DrawScroller.h"
#include "DrawDPAC.h"
using std::min;
using std::max;


int main( int argc, const char* argv[] )
{
    int demo = 0;
    int rot = 0;
    const char* filename = NULL;
    if (argc > 1) {
        demo = atoi(argv[1]);
    }
    
    if (argc > 2) {
        rot = atoi(argv[2]);
    }

    if(argc > 3)
    {
	filename = argv[3];
    }
    fprintf(stderr, "Using demo %d\n", demo);

    GPIO io;
    if (!io.Init())
    return 1;

    RGBMatrix m(&io);
    
    //m.setRotation(rot);
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
    case 5:
        image_gen = new ColorPulseGeneratorHSV(&m);
        break;
    case 6:
	image_gen = new DrawScroller(&m, filename);
	break;
    case 7:
	image_gen = new DrawDPAC(&m);
	break;
    default:
        printf( "\nNothing todo\n\n" );
        exit(0);
    }


    RGBMatrixManipulator *updater = new DisplayUpdater(&m);
    updater->Start(10);  // high priority
    if(image_gen != NULL)
    image_gen->Start();
    if(rot > 0)
    {
    	// Things are set up. Just wait for <RETURN> to be pressed.
    	printf("Press <RETURN> to exit and reset LEDs\n");
    	getchar();
    }
    else
    {
	while(!image_gen->isDone())
	{
	    usleep(10000);
	}
    }	

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
