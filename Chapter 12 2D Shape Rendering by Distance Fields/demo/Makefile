#Makefile for Windows mingw32, Linux and MacOSX (gcc environments)

Usage:
	@echo "Usage: make Win32 | Linux | MacOSX | clean | distclean"

Win32:
	gcc.exe -c contourdemo.c -o contourdemo.o -I. -IC:/Dev-Cpp/include -Wall -O3 -ffast-math -g3
	gcc.exe contourdemo.o -o contourdemo.exe -L. -mwindows -lglfw -lopengl32 -lglu32 -mconsole -g3

Linux:
	gcc -I. -I/usr/include contourdemo.c -lglfw -lGLU -o contourdemo

MacOSX:
	./bundle.sh contourdemo
	gcc -I. -I/usr/X11/include contourdemo.c -o contourdemo.app/Contents/MacOS/contourdemo -lglfw -framework Cocoa -framework OpenGL

clean:
	rm -f contourdemo.o

distclean:
	rm -rf contourdemo.o contourdemo contourdemo.exe contourdemo.app
