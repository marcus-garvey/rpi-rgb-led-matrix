#ifndef DrawRedAlert_H
#define DrawRedAlert_H

#include "RGBMatrixManipulator.h"

class DrawRedAlert : public RGBMatrixManipulator {
public:
	DrawRedAlert(RGBMatrix *m) : RGBMatrixManipulator(m) {}
	void Run();
};


#endif
