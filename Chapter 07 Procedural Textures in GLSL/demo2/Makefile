#Makefile for Windows mingw32, Linux and MacOSX (gcc environments)

Usage:
	@echo "Usage: make Win32 | Linux | MacOSX | clean | distclean"

Win32:
	gcc.exe -c shaderdemo.c -o shaderdemo.o -I. -IC:/Dev-Cpp/include -Wall -O3 -ffast-math -g3
	gcc.exe shaderdemo.o -o shaderdemo.exe -L. -mwindows -lglfw -lopengl32 -lglu32 -mconsole -g3

Linux:
	gcc -I. -I/usr/include shaderdemo.c -lglfw -lGLU -o shaderdemo

MacOSX:
	./bundle.sh shaderdemo
	gcc -I. -I/usr/X11/include shaderdemo.c -o shaderdemo.app/Contents/MacOS/shaderdemo -lglfw -framework Cocoa -framework OpenGL

clean:
	rm -f shaderdemo.o

distclean:
	rm -rf shaderdemo.o shaderdemo shaderdemo.exe shaderdemo.app
