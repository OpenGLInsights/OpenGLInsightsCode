This is a demo of animated procedural shaders, provided as
online supplementary material for the chapter "Procedural
Textures in GLSL" for the book "OpenGL Insights" published
by AK Peters in 2012. The book is copyrighted works subject
to its own license from the publisher, but this source code
is open and free, distributed under the MIT license.
See LICENSE.txt for details.

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

The program does not allow any interactions, but everything
you see on screen is rendered on the fly each frame, using
purely procedural GLSL fragment shaders. No textures, tables
or pre-computations are being used.

----
Stefan Gustavson (stefan.gustavson@liu.se), 2011-10-31
