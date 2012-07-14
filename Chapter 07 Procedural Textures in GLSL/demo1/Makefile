#Makefile for Windows mingw32, Linux and MacOSX (gcc environments)

Usage:
	@echo "Usage: make Win32 | Linux | MacOSX | clean | distclean"

Win32:
	gcc.exe -c shaderbench.c -o shaderbench.o -I. -IC:/Dev-Cpp/include -Wall -O3 -ffast-math -g3
	gcc.exe shaderbench.o -o shaderbench.exe -L. -mwindows -lglfw -lopengl32 -lglu32 -mconsole -g3

Linux:
	gcc -I. -I/usr/include shaderbench.c -lglfw -lGLU -o shaderbench

MacOSX:
	./bundle.sh shaderbench
	gcc -I. -I/usr/X11/include shaderbench.c -o shaderbench.app/Contents/MacOS/shaderbench -lglfw -framework Cocoa -framework OpenGL

clean:
	rm -f shaderbench.o

distclean:
	rm -rf shaderbench.o shaderbench shaderbench.exe shaderbench.app
