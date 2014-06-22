

#include "TestDrawers.h"
#include <unistd.h>

void SimpleSquare::Run() {
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

void ColorPulseGenerator::Run() {
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

      matrix_->fillRect(0,0,width,height,matrix_->Color888(r,g,b));
      matrix_->SwapScreen();
    }
}
