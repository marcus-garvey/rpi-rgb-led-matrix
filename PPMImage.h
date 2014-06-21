#ifndef PPMImage_H
#define PPMImage_H
#include <stdint.h>

#include <assert.h>
#include <unistd.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "led-matrix.h"

class PPMImage {
public:
	struct Pixel {
		uint8_t red;
		uint8_t green;
		uint8_t blue;
	};



	PPMImage(const char *filename);
	bool loadPPM(const char *filename);
	const Pixel &getPixel(int x, int y);

	void drawImage(RGBMatrix* matrix,int pos_x , int pos_y);

	int getWidth() { return width_; }
	int getHeight() { return height_; }
private:

	// Read line, skip comments.
	char *readLine(FILE *f, char *buffer, size_t len); 

	int width_;
	int height_;
	Pixel *image_;
};


#endif
