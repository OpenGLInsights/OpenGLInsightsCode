// Procedural shading demo, Stefan Gustavson 2011.
// This code is in the public domain.

uniform float texw, texh;
varying vec2 dims;  // In absence of textureSize()
varying vec2 one;
varying vec2 st;

void main( void )
{
  // Get the texture coordinates
  st = gl_MultiTexCoord0.xy;
  dims = vec2(texw, texh);
  one = 1.0 / dims; // Saves two divisions in the fragment shader
  gl_Position = ftransform();
}
