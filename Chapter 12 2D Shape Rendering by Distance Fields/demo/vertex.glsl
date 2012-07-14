// Distance map contour texturing according to Green (2007),
// implementation by Stefan Gustavson 2011.
// This code is in the public domain.

uniform float texw, texh;
varying float oneu, onev;
varying vec2 st;

void main( void )
{
  // Get the texture coordinates
  st = gl_MultiTexCoord0.xy;
  oneu = 1.0 / texw; // Save two divisions in the fragment shader
  onev = 1.0 / texh;
  gl_Position = ftransform();
}
