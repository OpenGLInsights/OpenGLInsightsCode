#Makefile for Windows mingw32, Linux and MacOSX (gcc environments)

Usage:
	@echo "Usage: make Win32 | Linux | MacOSX | clean | distclean"

Win32:
	gcc.exe -c GLSL-JFA.c -o GLSL-JFA.o -I. -IC:/Dev-Cpp/include -Wall -O3 -ffast-math -g3
	gcc.exe GLSL-JFA.o -o GLSL-JFA.exe -LC:/Dev-Cpp/lib -mwindows -lglfw -lopengl32 -lglu32 -mconsole -g3

Linux:
	gcc -I. -I/usr/X11/include GLSL-JFA.c -lglfw -o GLSL-JFA

MacOSX:
	./bundle.sh GLSL-JFA
	gcc -I. -I/usr/X11/include GLSL-JFA.c -o GLSL-JFA.app/Contents/MacOS/GLSL-JFA -lglfw -framework Cocoa -framework OpenGL

clean:
	rm -f GLSL-JFA.o

distclean:
	rm -rf GLSL-JFA.o GLSL-JFA GLSL-JFA.exe GLSL-JFA.app
