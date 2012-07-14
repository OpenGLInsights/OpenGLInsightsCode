#Makefile for Windows mingw32, Linux and MacOSX (gcc environments)

Usage:
	@echo "Usage: make Win32 | Linux | MacOSX | clean | distclean"

Win32:
	gcc.exe -c makedist.c -o makedist.o -I. -IC:/Dev-Cpp/include -Wall -O3 -ffast-math -g3
	gcc.exe makedist.o -o makedist.exe -L. -lm -mwindows -mconsole -g3

Linux:
	gcc -I. -I/usr/include makedist.c -lm -o makedist

MacOSX:
	gcc -I. makedist.c -lm -o makedist

clean:
	rm -f makedist.o

distclean:
	rm -f makedist.o makedist
