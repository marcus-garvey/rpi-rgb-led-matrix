#include "DrawScroller.h"
#include <unistd.h>
#include "DrawAnimation.h"
#include "PPMImage.h"

void DrawScroller::Run() {
    PPMImage *scroller = new PPMImage(filename_);
    const int imageW = scroller->getWidth();

    int pos = matrix_->displayWidth();
    while(running_)
    {
	matrix_->fillScreen(0);
	scroller->drawImage(matrix_,pos,0);
	
	//scroller->drawImage(matrix_,pos + imageW,0);
        pos--;
	if(pos < (-1 * imageW)) 
	{
		done_ = true;
		pos = matrix_->displayWidth();
	}
	matrix_->SwapScreen();
        usleep(10 * 1000);
    }

}
