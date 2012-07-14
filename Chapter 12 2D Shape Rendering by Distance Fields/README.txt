This is a demo program for the chapter "Contour Rendering
by Distance Fields" for the book "OpenGL Insights" published
by AK Peters in 2012. The book is copyrighted works subject
to its own license from the publisher, but this source code
is open and free, distributed in part as public domain code
and in part under the MIT license.
See LICENSE.txt for details.

Makefiles for GCC are included. Type "make" for instructions.
Both the code and the Makefile were designed to work under
Windows, MacOS X and Linux without changes, but the Makefile
was written for GNU Make and GCC, even under Windows. If you
want to use a different compiler or a different build system,
you are on your own.

Compilation requires the free GLFW library. A GCC library
archive file "libglfw.a" is distributed with the source, as
allowed by the GLFW license. For details on GLFW, see
http://http://www.glfw.org.

When running the program, press the number keys '1' through '4'
to switch between different shaders. Shader 1 is the main demo
showing the contour. Shader 2 displays the original grayscale
alpha image that was used to generate the distance field. Shader
3 is a composite of the two, to investigate details of the
rendering method. Shader 4 uses a standard bilinear interpolation
of the grayscale alpha texture followed by a thresholding, to
compare this method to the one that is still prevalent in
real time applications. Switch between 1 and 4 to see what
difference the distance field approach makes. Note that the
two shaders use similar amounts of data and involve similar
amounts of computations. The increase in quality comes
at little or no extra cost in resources or performance.
Note in particular the analytic anisotropic anti-aliasing at
grazing angles, with no unnecessary blurring regardless of
aspect ratio or viewing angle.

The view can be manipulated by the mouse. Drag to move,
shift-drag to zoom and ctrl-drag to rotate.

The texture creation tool "makedist" is a console application
with an old school command-line interface. You need to bring
up the console even under MacOS X.

----
Stefan Gustavson (stefan.gustavson@liu.se), 2012-06-12
