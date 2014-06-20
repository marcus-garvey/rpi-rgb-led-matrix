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
using std::min;
using std::max;



class SimpleSquare : public RGBMatrixManipulator {
public:
  SimpleSquare(RGBMatrix *m) : RGBMatrixManipulator(m) {}
  void Run() {
    const int width = matrix_->width();
    const int height = matrix_->height();
    // Diagonaly
    for (int x = 0; x < width; ++x) {
        matrix_->SetPixel(x, x, 255, 255, 255);
        matrix_->SetPixel(height -1 - x, x, 255, 0, 255);
    }
    for (int x = 0; x < width; ++x) {
      matrix_->SetPixel(x, 0, 255, 0, 0);
      matrix_->SetPixel(x, height - 1, 255, 255, 0);
    }
    for (int y = 0; y < height; ++y) {
      matrix_->SetPixel(0, y, 0, 0, 255);
      matrix_->SetPixel(width - 1, y, 0, 255, 0);
    }
    matrix_->SwapScreen();
  }
};

// Simple generator that pulses through RGB and White.
class ColorPulseGenerator : public RGBMatrixManipulator {
public:
  ColorPulseGenerator(RGBMatrix *m) : RGBMatrixManipulator(m) {}
  void Run() {
    const int width = matrix_->width();
    const int height = matrix_->height();
    uint32_t count = 0;
    while (running_) {
      usleep(5000);
      ++count;
      int color = (count >> 9) % 6;
      int value = count & 0xFF;
      if (count & 0x100) value = 255 - value;
      int r, g, b;
      switch (color) {
      case 0: r = value; g = b = 0; break;
      case 1: r = g = value; b = 0; break;
      case 2: g = value; r = b = 0; break;
      case 3: g = b = value; r = 0; break;
      case 4: b = value; r = g = 0; break;
      default: r = g = b = value; break;
      }
//      for (int x = 0; x < width; ++x)
//        for (int y = 0; y < height; ++y)
//          matrix_->SetPixel(x, y, r, g, b);

      matrix_->fillRect(0,0,64,16,matrix_->Color888(r,g,b));
      matrix_->SwapScreen();
    }
  }
};

class SimpleText : public RGBMatrixManipulator {
public:
  SimpleText(RGBMatrix *m) : RGBMatrixManipulator(m) {}
  void Run() {
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
};

class RedAlert : public RGBMatrixManipulator {
public:
  RedAlert(RGBMatrix *m) : RGBMatrixManipulator(m) {}
  void Run() {
    matrix_->setTextWrap(false);
    int low = 0;
    int high = 30;
    int higher = 255;
    int var1 = 0;
    int var2 = 0;
    int var3 = 0;
    int count = 0;
    while(running_)
    {	
        count++;
	switch(count)
	{
        case 1:
          var1 = var2 = var3 = low;
          break;
        case 10:
          var1 = high; 
	   var2 = var3 = low;
          break;
        case 20:
          var1 = var2 = high;
          var3 = low;
          break;
        case 30:
          var1 = var2 = var3 = high;
          break;
        case 40:
          var1 = higher;
           var2 = var3 = high;
          break;
	case 50:
          var1 = var2 = higher;
          var3 = high;
          break;
        case 60:
          var1 = var2 = var3 = higher;
          break;
        case 70:
          count = 0;
          break;
	}

        matrix_->fillScreen(0);
        matrix_->drawLine(0,0,63,0,matrix_->Color888(var1,0,0));
        matrix_->drawLine(3,2,60,2,matrix_->Color888(var2,0,0));
        matrix_->drawLine(3,12,60,12,matrix_->Color888(var2,0,0));
        matrix_->drawLine(0,14,63,14,matrix_->Color888(var1,0,0));
        matrix_->setTextColor(matrix_->Color888(var3,0,0));
        matrix_->setCursor(6, 4);
        matrix_->write("RED ALERT");
        matrix_->SwapScreen();
        usleep(30 * 1000);
    }
  }
};


class Animation : public RGBMatrixManipulator {
public:
  Animation(RGBMatrix *m) : RGBMatrixManipulator(m) {}
  void Run() {
	PPMImage *image[8];
	image[0] = new PPMImage("images/red1.ppm");
	image[1] = new PPMImage("images/red2.ppm");
	image[2] = new PPMImage("images/red3.ppm");
	image[3] = new PPMImage("images/red4.ppm");
	image[4] = new PPMImage("images/red5.ppm");
	image[5] = new PPMImage("images/red6.ppm");
	image[6] = new PPMImage("images/red7.ppm");
	image[7] = new PPMImage("images/red8.ppm");
        PPMImage *pac_full = new PPMImage("images/pac.ppm");
        PPMImage *pac_right_1 = new PPMImage("images/pac_right_1.ppm");
        PPMImage *pac_right_2 = new PPMImage("images/pac_right_2.ppm");
        PPMImage *pac_left_1 = new PPMImage("images/pac_left_1.ppm");
        PPMImage *pac_left_2 = new PPMImage("images/pac_left_2.ppm");
	PPMImage *pac_r[4];
	PPMImage *pac_l[4];
	pac_r[0] = pac_full;
	pac_r[1] = pac_right_2;
	pac_r[2] = pac_right_1;
	pac_r[3] = pac_right_2;

	pac_l[0] = pac_full;
	pac_l[1] = pac_left_2;
	pac_l[2] = pac_left_1;
	pac_l[3] = pac_left_2;

        int pac_image = 0;
	int count = 0;
	int count2 = 0;
	int count3 = 0;
	int pos = 0;
        bool dir_rev = false;
	while(running_)
	{
	   count2++;
           count3++;
	   if(count2 > 3)
	   {
	     count2 =0;
	     count++;
	     pac_image++;
	     if(count > 7) count = 0;
	     if(pac_image > 3) pac_image = 0;
	   }

	   if(count3 > 5)
	   {
	     count3 =0;
	     if(dir_rev)
		pos--;
	     else
	        pos++;
	   }
	   
	   if(pos == -40) dir_rev = false;
	   if(pos == 66) dir_rev = true;
	   
	   int pac_off = dir_rev ? 20 : 0 ;
	   int red_off = dir_rev ? 0 : 20;

	   image[count]->drawImage(matrix_,red_off + pos,0);
	   if(dir_rev)
	     pac_l[pac_image]->drawImage(matrix_,pac_off + pos,0);
	   else
	     pac_r[pac_image]->drawImage(matrix_,pac_off + pos,0);
	   matrix_->SwapScreen();
           usleep(20 * 1000);
	}

  }
};


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
	image_gen = new SimpleText(&m);
    break;
  case 1:
    image_gen = new SimpleSquare(&m);
    break;
  case 2:
    image_gen = new RedAlert(&m);
    break;
  case 3:
    image_gen = new ColorPulseGenerator(&m);
    break;
  case 4:
    image_gen = new Animation(&m);
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
