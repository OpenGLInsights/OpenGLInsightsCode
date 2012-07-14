CC=g++
CCFLAGS=-I. -I/usr/include -g -D_LINUX -rdynamic 
LIBS=-lglut -lGL -lGLEW -lIL -ltinyxml -L/usr/lib
FILES= vsFontLib.cpp \
	vsGLInfoLib.cpp \
	vsLogLib.cpp \
	vsShaderLib.cpp \
	vsMathLib.cpp \
	vsTerrainLib.cpp \
	vsResourceLib.cpp \
	vsTerrainLODSingleScaledLib.cpp \
	terrainTessDemo.cpp

main: $(FILES)
	$(CC) -o $@ $^ $(CCFLAGS) $(LIBS)

clean:
	rm -f *.o *~ main

