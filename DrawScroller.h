#ifndef DrawScroller_H
#define DrawScroller_H

#include "RGBMatrixManipulator.h"

class DrawScroller : public RGBMatrixManipulator {
public:
	const char * filename_;
	DrawScroller(RGBMatrix *m, const char * filename) : RGBMatrixManipulator(m), filename_(filename) {}
	void Run();
};

#endif
