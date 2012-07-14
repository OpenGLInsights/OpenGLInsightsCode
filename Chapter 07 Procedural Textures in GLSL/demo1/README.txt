This is the program used to benchmark various procedural shaders
for the chapter "Procedural Textures in GLSL" for the book
"OpenGL Insights" published by AK Peters in 2012.
The book is copyrighted works subject to its own license
from the publisher, but this source code is open and free,
distributed under the MIT license. See LICENSE.txt for details.

A Makefile for GCC is included. Type "make" for instructions.
Both the code and the Makefile were designed to work under
Windows, MacOS X and Linux without changes, but the Makefile
was written for GNU Make and GCC, even under Windows. If you
want to use a different compiler or a different build system,
you are on your own.

Compilation requires the free GLFW library. A GCC library
archive file "libglfw.a" is distributed with the source, as
allowed by the GLFW license. For details on GLFW, see
http://http://www.glfw.org.

When running the program, press the number keys '1' through '8'
to switch between different shaders. Shaders 1 and 2 are not
procedural, but provide a baseline for comparison. Shaders 3
through 7 are purely procedural. Shader 8 uses a texture as
input, and creates a strongly procedural pattern based on it.
The view can be manipulated by the mouse. Drag to move,
shift-drag to zoom and ctrl-drag to rotate.
The program launches with shader 8 active, to look nice.

----
Stefan Gustavson (stefan.gustavson@liu.se), 2011-10-31
