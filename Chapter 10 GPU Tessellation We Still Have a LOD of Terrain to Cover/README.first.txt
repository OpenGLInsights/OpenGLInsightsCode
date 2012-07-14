This demo shows the techniques described in the chapter
"GPU Tessellation: We still have a LOD of terrain to cover"
from the book "OpenGL Insights"

The code is provided as is, without any warranties.

The code must be edited to setup the terrain files (both texture and hight map)
and these should be squared and with dimensions that are multiple of 64.

A makefile and a VS2010 project are included.

This code depends on the following libs:

- GLEW to enable OpenGL 4 functionality
- Devil to load the height map and texture
- FreeGLUT 

Once running the code, several settings can be tested, namely the
LOD mode, the pixels per triangle, and usage of culling.

The terrain can also be viewed in wireframe so that the LOD effect can
be better grasped.





Keys:

LOD Mode
'y': Full tessellation
't': Simple Tessellation
'r' : Set Roughness LOD

Culling
'u': enable
'i': disable

Pixels per Triangle:
use +/- to increment/decrement this value

Polygon Mode
'l': GL_LINE
'f': GL_FILL


Have fun,

António and Bruno
antonio.ramires@gmail.com
indie.mail@gmail.com