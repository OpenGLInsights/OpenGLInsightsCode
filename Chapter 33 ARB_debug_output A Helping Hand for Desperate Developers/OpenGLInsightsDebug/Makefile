CC=g++
CCFLAGS=-I. -I/usr/include -g -D_LINUX -rdynamic
LIBS=-lunwind -lglut -lGL -lGLEW -L/usr/lib
FILES = vsDebugLib.cpp ogl.cpp

main: $(FILES)
	$(CC) -o $@ $^ $(CCFLAGS) $(LIBS)

clean:
	rm -f *.o *~ main