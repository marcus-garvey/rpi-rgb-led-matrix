#include "DrawAnimation.h"
#include <unistd.h>
#include "PPMImage.h"

void DrawAnimation::Run() {
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
        if(pos == 100) dir_rev = true;

        int pac_off = dir_rev ? 20 : 0 ;
        int red_off = dir_rev ? 0 : 20;

        image[count]->drawImage(matrix_,red_off + pos,0);
	image[count]->drawImage(matrix_,25,16);
	image[count]->drawImage(matrix_,43,16);
	image[count]->drawImage(matrix_,62,16);
        if(dir_rev)
        pac_l[pac_image]->drawImage(matrix_,pac_off + pos,0);
        else
        pac_r[pac_image]->drawImage(matrix_,pac_off + pos,0);
        matrix_->SwapScreen();
        usleep(20 * 1000);
    }

}

