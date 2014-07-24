#include "DrawDPAC.h"
#include <unistd.h>
#include "PPMImage.h"

void DrawDPAC::Run() {
    PPMImage *image[8];
    image[0] = new PPMImage("images/dpac/DPAC_FULL.ppm");
    image[1] = new PPMImage("images/dpac/DPAC_HO.ppm");
    image[2] = new PPMImage("images/dpac/DPAC_FO.ppm");
    image[3] = image[1];
    PPMImage *enemy = new PPMImage("images/enemy.ppm");
    int count = 0;
    int count2 = 0;
    int pac_pos = -32;
    while(running_)
    {
	matrix_->fillScreen(0);
	int flag_y = 7;

	matrix_->fillRect(pac_pos + 16,flag_y,120,5,matrix_->Color888(0,0,255));
	matrix_->fillRect(pac_pos + 16,flag_y+5,120,5,matrix_->Color888(255,255,255));
	matrix_->fillRect(pac_pos + 16,flag_y+10,120,5,matrix_->Color888(0,0,255));
	if(pac_pos < 80)
		enemy->drawImage(matrix_,80,8);
	image[count]->drawImage(matrix_,pac_pos,0);

	count2++;
	if(count2 > 5)
	{
	    count++;
	    if(count > 3) count = 0 ;
	    count2 = 0;
	}
	pac_pos++;
	if(pac_pos > 96) {
	    pac_pos = -32;
	    done_ = true;
	}

        matrix_->SwapScreen();
        usleep(20 * 1000);
    }

}

