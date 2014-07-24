// Some experimental code.
// (c) H. Zeller <h.zeller@acm.org>. License: do whatever you want with it :)
// 2013-12 - Modified for a 16x32 matrix (half the original panel)
//
// Using GPIO to control a 16x32 rgb LED panel (typically you find them with the
// suffix such as P4 or P5: that is the pitch in mm.
// So "32x32 rgb led p5" should find you something on 'the internets'.

#include "led-matrix.h"

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "gpio.h"

// Clocking in a row takes about 3.4usec (TODO: this is actually per board)
// Because clocking the data in is part of the 'wait time', we need to
// substract that from the row sleep time.
static const int kRowClockTime = 3400;
static const int kBaseTime = kRowClockTime;  // smallest possible value.

static const uint8_t gamma[] = {
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x01,0x01,0x01,0x01,0x01,0x01,
    0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
    0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
    0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
    0x01,0x01,0x01,0x01,0x01,0x01,0x02,0x02,
    0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,
    0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,
    0x02,0x02,0x02,0x02,0x02,0x03,0x03,0x03,
    0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,
    0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x04,
    0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,
    0x04,0x04,0x04,0x04,0x04,0x04,0x05,0x05,
    0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,
    0x05,0x05,0x05,0x06,0x06,0x06,0x06,0x06,
    0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x07,
    0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,
    0x07,0x07,0x08,0x08,0x08,0x08,0x08,0x08,
    0x08,0x08,0x08,0x08,0x09,0x09,0x09,0x09,
    0x09,0x09,0x09,0x09,0x09,0x0a,0x0a,0x0a,
    0x0a,0x0a,0x0a,0x0a,0x0a,0x0a,0x0b,0x0b,
    0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0c,0x0c,
    0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0d,0x0d,
    0x0d,0x0d,0x0d,0x0d,0x0d,0x0e,0x0e,0x0e,
    0x0e,0x0e,0x0e,0x0e,0x0f,0x0f,0x0f,0x0f
};

const long row_sleep_nanos[8] = {   // Only using the first kPWMBits elements.
    (1 * kBaseTime) - kRowClockTime,
    (2 * kBaseTime) - kRowClockTime,
    (4 * kBaseTime) - kRowClockTime,
    (8 * kBaseTime) - kRowClockTime,
    (16 * kBaseTime) - kRowClockTime,
    (32 * kBaseTime) - kRowClockTime,
    (64 * kBaseTime) - kRowClockTime,
    // Too much flicker with 8 bits. We should have a separate screen pass
    // with this bit plane. Or interlace. Or trick with -OE switch on in the
    // middle of row-clocking, thus have kRowClockTime / 2
    (128 * kBaseTime) - kRowClockTime, // too much flicker.
};

static void sleep_nanos(long nanos) {
    // For sleep times above 20usec, nanosleep seems to be fine, but it has
    // an offset of about 20usec (on the RPi distribution I was testing it on).
    // That means, we need to give it 80us to get 100us.
    // For values lower than roughly 30us, this is not accurate anymore and we
    // need to switch to busy wait.
    // TODO: compile Linux kernel realtime extensions and watch if the offset-time
    // changes and hope for less jitter.
    if (nanos > 28000) {
        struct timespec sleep_time = { 0, nanos - 20000 };
        nanosleep(&sleep_time, NULL);
    } else {
        // The following loop is determined empirically on a 700Mhz RPi
        for (int i = nanos >> 2; i != 0; --i) {
            asm("");   // force GCC not to optimize this away.
        }
    }
}

int RGBMatrix::displayWidth() 
{
    if(panelSetup == LeftToRightHalfUpToDown)
    {
        return kPanelColumns * (kChainedBoards/2);
    }
    else if(panelSetup == UpToDown)
    {
        return kPanelColumns;
    }
    // LeftToRight
    return kPanelColumns *  kChainedBoards;
}

int RGBMatrix::displayHeight() 
{
    if(panelSetup == LeftToRightHalfUpToDown)
    {
        return kPanelRows * 2;
    }
    else if(panelSetup == UpToDown)
    {
        return kPanelRows * kChainedBoards;
    }
    // LeftToRight
    return kPanelRows;
}

RGBMatrix::RGBMatrix(GPIO *io ) : 
Adafruit_GFX(displayWidth(),displayHeight()),
io_(io),
backindex_(0),
swapbuffer_(false)
{
    printf("Create x=%d y=%d display\n",displayWidth(),displayHeight());
    // Tell GPIO about all bits we intend to use.
    IoBits b;
    b.raw = 0;
    b.bits.output_enable = b.bits.clock = b.bits.strobe = 1;
    b.bits.r1 = b.bits.g1 = b.bits.b1 = 1;
    b.bits.r2 = b.bits.g2 = b.bits.b2 = 1;
    b.bits.row = 0x7; // 8 rows, 0-based

    // Initialize outputs, make sure that all of these are supported bits.
    const uint32_t result = io_->InitOutputs(b.raw);
    assert(result == b.raw);
    assert(kPWMBits < 8);    // only up to 7 makes sense.
    ClearScreen();
}

RGBMatrix::~RGBMatrix()
{
}
// Demote 8/8/8 to Adafruit_GFX 5/6/5
// If no gamma flag passed, assume linear color
uint16_t RGBMatrix::Color888(uint8_t r, uint8_t g, uint8_t b) {
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8 ) >> 3);
    //return ((r & 0xF8) << 11) | ((g & 0xFC) << 5) | (b >> 3);
}
// 8/8/8 -> gamma -> 5/6/5
uint16_t RGBMatrix::Color888Gamma(uint8_t r, uint8_t g, uint8_t b) 
{
    r = gamma[r]; // Gamma correction table maps
    g = gamma[g]; // 8-bit input to 4-bit output
    b = gamma[b];
    return (r << 12) | ((r & 0x8) << 8) | // 4/4/4 -> 5/6/5
    (g <<  7) | ((g & 0xC) << 3) |
    (b <<  1) | ( b        >> 3);

}
uint16_t RGBMatrix::ColorHSV(long hue, uint8_t sat, uint8_t val, boolean gflag) {

    uint8_t  r, g, b, lo;
    uint16_t s1, v1;

    // Hue
    hue %= 1536;             // -1535 to +1535
    if(hue < 0) hue += 1536; //     0 to +1535
    lo = hue & 255;          // Low byte  = primary/secondary color mix
    switch(hue >> 8) {       // High byte = sextant of colorwheel
    case 0 : r = 255     ; g =  lo     ; b =   0     ; break; // R to Y
    case 1 : r = 255 - lo; g = 255     ; b =   0     ; break; // Y to G
    case 2 : r =   0     ; g = 255     ; b =  lo     ; break; // G to C
    case 3 : r =   0     ; g = 255 - lo; b = 255     ; break; // C to B
    case 4 : r =  lo     ; g =   0     ; b = 255     ; break; // B to M
    default: r = 255     ; g =   0     ; b = 255 - lo; break; // M to R
    }

    // Saturation: add 1 so range is 1 to 256, allowig a quick shift operation
    // on the result rather than a costly divide, while the type upgrade to int
    // avoids repeated type conversions in both directions.
    s1 = sat + 1;
    r  = 255 - (((255 - r) * s1) >> 8);
    g  = 255 - (((255 - g) * s1) >> 8);
    b  = 255 - (((255 - b) * s1) >> 8);

    // Value (brightness) & 16-bit color reduction: similar to above, add 1
    // to allow shifts, and upgrade to int makes other conversions implicit.
    v1 = val + 1;
    if(gflag) { // Gamma-corrected color?
        r = gamma[(r * v1) >> 8]; // Gamma correction table maps
        g = gamma[(g * v1) >> 8]; // 8-bit input to 4-bit output
        b = gamma[(b * v1) >> 8];
    } else { // linear (uncorrected) color
        r = (r * v1) >> 12; // 4-bit results
        g = (g * v1) >> 12;
        b = (b * v1) >> 12;
    }
    return (r << 12) | ((r & 0x8) << 8) | // 4/4/4 -> 5/6/5
    (g <<  7) | ((g & 0xC) << 3) |
    (b <<  1) | ( b        >> 3);
}

void RGBMatrix::ClearScreen() {
    memset(&bitplane_[1 - backindex_], 0, sizeof(bitplane_[1 - backindex_]));
}
void RGBMatrix::drawPixel(int16_t x, int16_t y, uint16_t c) {

    // Adafruit_GFX uses 16-bit color in 5/6/5 format, while matrix needs
    // 4/4/4.  Pluck out relevant bits while separating into R,G,B:
    uint8_t r = (c >> 11) & 0x1F;        // RRRRrggggggbbbbb
    uint8_t g = (c >>  5) & 0x3F; // rrrrrGGGGggbbbbb
    uint8_t b = c & 0x1F; // rrrrrggggggBBBBb

    SetPixel(x,y,r <<3,g <<2,b << 3);
    //SetPixel(x,y,r ,g,b );
}

void RGBMatrix::SetPixel(int x, int y, uint8_t red, uint8_t green, uint8_t blue)
{

    // here we calc the rotation
    switch(rotation) {
    case 1:
        swap(x, y);
        x = WIDTH  - 1 - x;
        break;
    case 2:
        x = WIDTH  - 1 - x;
        y = HEIGHT - 1 - y;
        break;
    case 3:
        swap(x, y);
        y = HEIGHT - 1 - y;
        break;
    }
    
    internalSetPixel(x,y,red,green,blue);
}

void RGBMatrix::internalSetPixel(int x, int y, uint8_t red, uint8_t green, uint8_t blue) 
{
    if (x < 0 || x >= displayWidth() || y < 0 || y >= displayHeight()) return;
    // My setup: A single panel connected  [>] 16 rows & 32 columns.
    uint8_t coord_x = x;
    uint8_t coord_y = y;
    if(panelSetup == LeftToRightHalfUpToDown)
    {
        int halfChainedPanel = kChainedBoards /2;
        if(coord_y >= kPanelRows )
        {
            coord_y = (y % kPanelRows);
            coord_x = (halfChainedPanel * kPanelColumns) + x;
            // now flipp
            coord_y = (kPanelRows  -1) - coord_y;
            coord_x = (kPanelColumns * kChainedBoards) - (coord_x % ( kPanelColumns * halfChainedPanel)) -1;
        }
    }
    else if(panelSetup == UpToDown)
    {
        int panel = (y / kPanelRows); 
        //printf("Panel %d\n" , panel);
        // x = 0 & y = 15 -> x = 0 & y = 15
        // x = 0 & y = 32 -> x = 64 & y = 0  //  pos not flipped
        // x = 1 & y = 32 -> x = 65 & y = 0  //  pos not flipped
        coord_y = (y % kPanelRows);
        coord_x = (panel * kPanelColumns) + x;
        //printf("Org Coord  x=%d y=%d Calc Coord x=%d y=%d\n",x,y,coord_x,coord_y);
        if(!(panel == 0 || (panel % 2) == 0))
        {
            // x = 0 & y = 16 -> x = 63 & y = 15 //  pos flipped
            // x = 1 & y = 48 -> x = ? & y = ? // 
            coord_y = (kPanelRows -1) - coord_y;
            coord_x = kPanelColumns * (panel+1) - (coord_x % kPanelColumns) -1;
            //printf("Flipped Calc Coord x=%d y=%d\n",coord_x,coord_y);
        }
        
    }
    // TODO: re-map values to be luminance corrected (sometimes called 'gamma').
    // Ideally, we had like 10PWM bits for this, but we're too slow for that :/

    // Scale to the number of bit planes we actually have, so that MSB matches
    // MSB of PWM.
    red   >>= 8 - kPWMBits;
    green >>= 8 - kPWMBits;
    blue  >>= 8 - kPWMBits;

    for (int b = 0; b < kPWMBits; ++b) {
        uint8_t mask = 1 << b;
        IoBits *bits = &bitplane_[1 - backindex_][b].row[coord_y & 0x7].column[coord_x];  // 8 rows, 0-based
        if (coord_y < 8) {    // Upper sub-panel. - 16 actual rows; 8 high & 8 low
            bits->bits.r1 = (red & mask) == mask;
            bits->bits.g1 = (green & mask) == mask;
            bits->bits.b1 = (blue & mask) == mask;
        } else {        // Lower sub-panel.
            bits->bits.r2 = (red & mask) == mask;
            bits->bits.g2 = (green & mask) == mask;
            bits->bits.b2 = (blue & mask) == mask;
        }
    }
}

void RGBMatrix::SwapScreen(bool wait)
{
    swapbuffer_ = true;

    if(!wait) return;

    while(swapbuffer_)
    {
        usleep(100);
    }
}

void RGBMatrix::UpdateScreen()
{
    if(swapbuffer_)
    {
        backindex_ = 1 - backindex_;
        swapbuffer_ = false;
    }


    IoBits serial_mask;   // Mask of bits we need to set while clocking in.
    serial_mask.bits.r1 = serial_mask.bits.g1 = serial_mask.bits.b1 = 1;
    serial_mask.bits.r2 = serial_mask.bits.g2 = serial_mask.bits.b2 = 1;
    serial_mask.bits.clock = 1;

    IoBits row_mask;
    row_mask.bits.row = 0x7;  // 8 rows, 0-based

    IoBits clock, output_enable, strobe;    
    clock.bits.clock = 1;
    output_enable.bits.output_enable = 1;
    strobe.bits.strobe = 1;

    IoBits row_bits;
    for (uint8_t row = 0; row < kDoubleRows; ++row) {
        // Rows can't be switched very quickly without ghosting, so we do the
        // full PWM of one row before switching rows.
        for (int b = 0; b < kPWMBits; ++b) {
            const DoubleRow &rowdata = bitplane_[backindex_][b].row[row];

            // Clock in the row. The time this takes is the smalles time we can
            // leave the LEDs on, thus the smallest time-constant we can use for
            // PWM (doubling the sleep time with each bit).
            // So this is the critical path; I'd love to know if we can employ some
            // DMA techniques to speed this up.
            // (With this code, one row roughly takes 3.0 - 3.4usec to clock in).
            //
            // However, in particular for longer chaining, it seems we need some more
            // wait time to settle.
            const long kIOStabilizeWaitNanos = 256;
            for (uint8_t col = 0; col < kColumns; ++col) {
                const IoBits &out = rowdata.column[col];
                io_->ClearBits(~out.raw & serial_mask.raw);  // also: resets clock.
                sleep_nanos(kIOStabilizeWaitNanos);
                io_->SetBits(out.raw & serial_mask.raw);
                sleep_nanos(kIOStabilizeWaitNanos);
                io_->SetBits(clock.raw);
                sleep_nanos(kIOStabilizeWaitNanos);
            }

            io_->SetBits(output_enable.raw);  // switch off while strobe.

            row_bits.bits.row = row;
            io_->SetBits(row_bits.raw & row_mask.raw);
            io_->ClearBits(~row_bits.raw & row_mask.raw);

            io_->SetBits(strobe.raw);   // Strobe
            io_->ClearBits(strobe.raw);

            // Now switch on for the given sleep time.
            io_->ClearBits(output_enable.raw);
            // If we use less bits, then use the upper areas which leaves us more
            // CPU time to do other stuff.
            sleep_nanos(row_sleep_nanos[b + (7 - kPWMBits)]);
        }
    }
}
