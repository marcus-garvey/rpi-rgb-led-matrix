#include "DrawSimpleText.h"
#include <unistd.h>

void DrawSimpleText::Run() {
    const int width = matrix_->width();
    const int height = matrix_->height();
    int8_t ball[3][4] = {
  	{  3,  0,  1,  1 }, // Initial X,Y pos & velocity for 3 bouncy balls
  	{ 17, 15,  1, -1 },
  	{ 27,  4, -1,  1 }
    };
    int16_t ballcolor[] = {matrix_->Color888(255,0,0),matrix_->Color888(0,255,0),matrix_->Color888(0,0,255)};

    matrix_->setTextSize(2);
    matrix_->setTextWrap(false);
    int move = width;
    int r = 255;
    int g = 255;
    int b = 255;
    while(running_)
    {	
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	//char * text = "TEST";
	char text[100];
        //sprintf(text,"I love you!!");
	sprintf(text,"Time %d:%d:%d", tm.tm_hour, tm.tm_min, tm.tm_sec);


        matrix_->fillScreen(0);

	// Bounce three balls around
  	for(int i=0; i<3; i++) {
    		// Draw 'ball'
    		matrix_->fillCircle(ball[i][0], ball[i][1], 5, ballcolor[i]);
    		// Update X, Y position
    		ball[i][0] += ball[i][2];
    		ball[i][1] += ball[i][3];
    		// Bounce off edges
    		if((ball[i][0] == 0) || (ball[i][0] == (width - 1)))
      			ball[i][2] *= -1;
    		if((ball[i][1] == 0) || (ball[i][1] == (height - 1)))
      			ball[i][3] *= -1;
  	}


	matrix_->setTextColor(matrix_->Color888(r,g,b));
        matrix_->setCursor(move, 1);
        matrix_->write(text);
        if(--move < (-12 * 14)) move = width;

	//r += 1;
	//g += 2;
        //b += 3;
	//if(r > 255) r = 0;
	//if(g > 255) g = 0;
	//if(b > 255) b = 0;
	matrix_->SwapScreen();
        usleep(30 * 1000);
    }
}
