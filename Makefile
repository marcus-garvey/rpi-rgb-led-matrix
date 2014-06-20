CFLAGS  = -Wall -g 
LDFLAGS = -lrt -lm -lpthread 
BINARIES= ledmatrix
OBJ = main.o Adafruit_GFX.o led-matrix.o gpio.o thread.o PPMImage.o
all: $(BINARIES)

ledmatrix: $(OBJ)
	$(CXX) $(CFLAGS) -o ledmatrix $(OBJ) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CFLAGS) -c $<

%.o: %.cc
	$(CXX) $(CFLAGS) -c $<

clean:
	rm -f $(OBJ) $(BINARIES)
