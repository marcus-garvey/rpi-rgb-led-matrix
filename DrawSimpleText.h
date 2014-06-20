#ifndef DrawSimpleText_H
#define DrawSimpleText_H

#include "RGBMatrixManipulator.h"

class DrawSimpleText : public RGBMatrixManipulator {
public:
  DrawSimpleText(RGBMatrix *m) : RGBMatrixManipulator(m) {}
  void Run();
};

#endif
