// -*- c++ -*-
// Controlling a 16x32 RGB matrix via GPIO.

#ifndef RPI_RGBMATRIX_H
#define RPI_RGBMATRIX_H

#include <stdint.h>
#include "gpio.h"
#include "Adafruit_GFX.h"

//define CHAINED_BOARDS 2
#define BOARD_WIDTH_X 32
#define BORAD_HEIGHT_Y 16

class RGBMatrix : public Adafruit_GFX {
 public:
  RGBMatrix(GPIO *io);
  void ClearScreen();

  int width() const { return CHAINED_BOARDS * BOARD_WIDTH_X; }
  int height() const { return BORAD_HEIGHT_Y; }
  void drawPixel(int16_t x, int16_t y, uint16_t c);
  void SetPixel(uint8_t x, uint8_t y, uint8_t red, uint8_t green, uint8_t blue);
uint16_t
    Color888(uint8_t r, uint8_t g, uint8_t b);
  // Updates the screen once. Call this in a continous loop in some realtime
  // thread.
  void UpdateScreen();
  void SwapScreen(bool wait = true);

private:
  GPIO *const io_;
  int backindex_;
  bool swapbuffer_;

  enum {
    kDoubleRows = 8,     // Physical constant of the used board.
    kChainedBoards = CHAINED_BOARDS,   // Number of boards that are daisy-chained.
    kColumns = kChainedBoards * BOARD_WIDTH_X,
    kPWMBits = 4          // maximum PWM resolution.
  };


  union IoBits {
    struct {
      unsigned int unused1 : 2;  // 0..1
      unsigned int output_enable : 1;  // 2
      unsigned int clock  : 1;   // 3
      unsigned int strobe : 1;   // 4
      unsigned int unused2 : 2;  // 5..6
      unsigned int row : 3;  // 7..9
      unsigned int unused3 : 7;  // 10..16
      unsigned int r1 : 1;   // 17
      unsigned int g1 : 1;   // 18
      unsigned int unused4 : 3;
      unsigned int b1 : 1;   // 22
      unsigned int r2 : 1;   // 23
      unsigned int g2 : 1;   // 24
      unsigned int b2 : 1;   // 25
    } bits;
    uint32_t raw;
    IoBits() : raw(0) {}
  };

  // A double row represents row n and n+8. The physical layout of the
  // 16x32 RGB is two sub-panels with 32 columns and 8 rows.
  struct DoubleRow {
    IoBits column[kColumns];  // only color bits are set
  };
  struct Screen {
    DoubleRow row[kDoubleRows];
  };

  Screen bitplane_[2][kPWMBits];
};

#endif  // RPI_RGBMATRIX_H
