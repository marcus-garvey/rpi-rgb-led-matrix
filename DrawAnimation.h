#ifndef DrawAnimation_H
#define DrawAnimation_H
#include "RGBMatrixManipulator.h"


class DrawAnimation : public RGBMatrixManipulator {
public:
	DrawAnimation(RGBMatrix *m) : RGBMatrixManipulator(m) {}
	void Run();
};

#endif
