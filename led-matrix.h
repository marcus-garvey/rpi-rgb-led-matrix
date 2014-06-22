// -*- c++ -*-
// Controlling a 16x32 RGB matrix via GPIO.

#ifndef RPI_RGBMATRIX_H
#define RPI_RGBMATRIX_H

#include <stdint.h>
#include "gpio.h"
#include "Adafruit_GFX.h"

//define CHAINED_BOARDS 2
#define BOARD_WIDTH_X 32
#define BOARD_HEIGHT_Y 16

class RGBMatrix : public Adafruit_GFX {
public:
    // display configuration
    // 
    enum PanelSetup {
        LeftToRight,
        LeftToRightHalfUpToDown,
        UpToDown
    };
    // some general setup constants
    const static int kPanelRows = BOARD_HEIGHT_Y;
    const static int kPanelColumns = BOARD_WIDTH_X;
    const static int kChainedBoards = CHAINED_BOARDS;
    const static PanelSetup panelSetup = LeftToRightHalfUpToDown;
    // hardware setup of the display memory
    const static int kDoubleRows = 8;
    const static int kColumns = kChainedBoards * kPanelColumns;
    const static int kPWMBits = 4;
    
    
    static int displayWidth(); //{ return kColumns; }
    static int displayHeight(); //{ return BORAD_HEIGHT_Y; }
    
    RGBMatrix(GPIO *io);
    ~RGBMatrix();
    
    void ClearScreen();

    void drawPixel(int16_t x, int16_t y, uint16_t c);
    void SetPixel(uint8_t x, uint8_t y, uint8_t red, uint8_t green, uint8_t blue);
    uint16_t Color888(uint8_t r, uint8_t g, uint8_t b);
    uint16_t Color888Gamma(uint8_t r, uint8_t g, uint8_t b) ;
    uint16_t ColorHSV(long hue, uint8_t sat, uint8_t val, boolean gflag = true);
    // Updates the screen once. Call this in a continous loop in some realtime
    // thread.
    void UpdateScreen();
    void SwapScreen(bool wait = true);

private:
    
    void internalSetPixel(uint8_t x, uint8_t y, uint8_t red, uint8_t green, uint8_t blue);
    
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
    // A double row represents row n and n+8. The physical layout of the
    // 16x32 RGB is two sub-panels with 32 columns and 8 rows.
    struct DoubleRow {
        IoBits column[kColumns];  // only color bits are set
    };
    struct Screen {
        DoubleRow row[kDoubleRows];
    };

    
    Screen bitplane_[2][kPWMBits];
    GPIO *const io_;
    int backindex_;
    bool swapbuffer_;
    
};

#endif  // RPI_RGBMATRIX_H
