#ifndef DrawDPAC_H
#define DrawDPAC_H
#include "RGBMatrixManipulator.h"


class DrawDPAC : public RGBMatrixManipulator {
public:
    DrawDPAC(RGBMatrix *m) : RGBMatrixManipulator(m) {}
    void Run();
};

#endif
