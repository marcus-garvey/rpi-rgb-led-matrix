#ifndef RGBMatrixManipulator_H
#define RGBMatrixManipulator_H
#include "led-matrix.h"
#include "thread.h"

// Base-class for a Thread that does something with a matrix.
class RGBMatrixManipulator : public Thread {
public:
    RGBMatrixManipulator(RGBMatrix *m) : running_(true), matrix_(m) {}
    virtual ~RGBMatrixManipulator() { running_ = false; }

    // Run() implementation needs to check running_ regularly.

protected:
    volatile bool running_;  // TODO: use mutex, but this is good enough for now.
    RGBMatrix *const matrix_;
};


// Pump pixels to screen. Needs to be high priority real-time because jitter
// here will make the PWM uneven.
class DisplayUpdater : public RGBMatrixManipulator {
public:
    DisplayUpdater(RGBMatrix *m) : RGBMatrixManipulator(m) {}

    void Run() {
        while (running_) {
            matrix_->UpdateScreen();
        }
    }
};



#endif
