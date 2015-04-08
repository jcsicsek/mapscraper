#Makefile for Mapscraper
#Jeff Csicsek
#March 2012

#paths are for Mac OS X Lion with OpenCV installed via MacPorts

CXX=g++
CXXFLAGS = -I"/opt/local/include/opencv" -g
LINKFLAGS = -L"/opt/local/lib" -lopencv_core -lopencv_imgproc -lopencv_calib3d -lopencv_video -lopencv_features2d -lopencv_ml -lopencv_highgui -lopencv_objdetect -lopencv_contrib -lopencv_legacy
SRC =  engine.cpp map.cpp
OBJ = $(SRC:.cpp=.o)

all: mapscraper

mapscraper: $(OBJ)
	@$(CXX) $(CXXFLAGS) -o $@ $(LINKFLAGS) $(OBJ)

clean:
	/bin/rm -rf *.o *.dSYM mapscraper

