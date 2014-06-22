#include "DrawRedAlert.h"
#include <unistd.h>

void DrawRedAlert::Run() {
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


