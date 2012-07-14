This is a library of GLSL functions for procedural shading,
provided as online supplementary material for the chapter
"Procedural Textures in GLSL" for the book "OpenGL Insights"
published by AK Peters in 2012. The book is copyrighted works
subject to its own license from the publisher, but this source
code is open and free, distributed under the MIT license.
See LICENSE.txt for details.

These functions are all self-contained. To use them, simply
cut and paste the code in any of the source files into a
vertex or fragment shader, and call the corresponding
function. Note, however, that if you include several of
these functions in the same shader, some helper functions
might be redefined, and the compilation will fail.
If that happens, simply find the redundant repeated
definition and delete it. The possibly redefined functions
are "mod289()" and "permute()".

The functions in this library are:

Perlin simplex noise over 2, 3 and 4 dimensions:

float snoise(vec2 point) in file "snoise2.glsl"
float snoise(vec3 point) in file "snoise3.glsl"
float snoise(vec4 point) in file "snoise4.glsl"

Rotating gradient simplex noise in 2D, to create
"flow noise" according to Perlin and Neyret:

float srdnoise(in vec2 point, in float rotation, out vec2 gradient)
in file "srdnoise2.glsl"

Perlin classic noise over 2, 3 and 4 dimensions:

float cnoise(vec2 point) in file "cnoise2.glsl"
float cnoise(vec3 point) in file "cnoise3.glsl"
float cnoise(vec4 point) in file "cnoise4.glsl"

Perlin classic noise over 2, 3 and 4 dimensions
and a specified repetition period other than the
default 289:

float pnoise(vec2 point, vec2 period) in file "cnoise2.glsl"
float pnoise(vec3 point, vec3 period) in file "cnoise3.glsl"
float pnoise(vec4 point, vec4 period) in file "cnoise4.glsl"

Worley cellular noise over 2 and 3 dimensions:

vec2 cellular(vec2 point) in file "cellular2.glsl"
vec2 cellular(vec3 point) in file "cellular3.glsl"

Faster Worley cellular noise with slight errors:

float cellular2x2(vec2 point) in file "cellular2x2.glsl"
float cellular2x2x2(vec3 point) in file "cellular2x2x2.glsl"

----
Stefan Gustavson (stefan.gustavson@liu.se), 2012-06-12
