CFLAGS  = -Wall -g -DCHAINED_BOARDS=2
LDFLAGS = -lrt -lm -lpthread 
BINARIES= ledmatrix
OBJ = main.o Adafruit_GFX.o led-matrix.o gpio.o thread.o PPMImage.o RGBMatrixManipulator.o TestDrawers.o DrawSimpleText.o DrawRedAlert.o DrawAnimation.o
all: $(BINARIES)

ledmatrix: $(OBJ)
	$(CXX) $(CFLAGS) -o ledmatrix $(OBJ) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CFLAGS) -c $<

%.o: %.cc
	$(CXX) $(CFLAGS) -c $<

clean:
	rm -f $(OBJ) $(BINARIES)
