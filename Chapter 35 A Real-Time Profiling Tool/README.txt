This code is an implementation of the graphical real-time profiler using OpenGL timer queries and precise OS-specific CPU time queries that is described in the chapter "A real-time profiling tool" from the book "OpenGL Insights".
It is covered by the WTFPL License.
You need an OpenGL 3.3 compatible GPU and driver to run it.
It depends on the libraries GLEW and GLFW.

To compile with Visual C++ 2010, use the provided project files.
To compile with the MinGW compiler, use the command:
	make -f Makefile.mingw
	
To compile on MacOS X, use the command:
	make -f Makefile.osx
	
To compile on Linux, you need to install SCons, GLEW and GLFW, and type:
	scons
	
This code is provided in the hope that it will be useful.
Patches and improvements are welcome. The code is also available on the following Git repository:
	https://github.com/Funto/OpenGL-Timestamp-Profiler
	
Lionel Fuentes.
