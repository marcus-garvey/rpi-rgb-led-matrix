#ifndef TestDrawers_H
#define TestDrawers_H

#include "RGBMatrixManipulator.h"

class SimpleSquare : public RGBMatrixManipulator {
public:
  SimpleSquare(RGBMatrix *m) : RGBMatrixManipulator(m) {}
  void Run();
};

// Simple generator that pulses through RGB and White.
class ColorPulseGenerator : public RGBMatrixManipulator {
public:
  ColorPulseGenerator(RGBMatrix *m) : RGBMatrixManipulator(m) {}
  void Run();
};

// Simple generator that pulses through RGB and White.
class ColorPulseGeneratorHSV : public RGBMatrixManipulator {
public:
  ColorPulseGeneratorHSV(RGBMatrix *m) : RGBMatrixManipulator(m) {}
  void Run();
};
#endif
