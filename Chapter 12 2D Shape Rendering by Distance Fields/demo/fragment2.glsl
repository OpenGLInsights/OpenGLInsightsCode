// Distance map contour texturing, Stefan Gustavson 2011
// A re-implementation of Green's method, with a 16-bit
// 8.8 distance map and explicit bilinear interpolation.
// This code is in the public domain.

uniform sampler2D disttex;
uniform float texw, texh;
varying float oneu, onev;
varying vec2 st;

void main( void )
{
  gl_FragColor = vec4(texture2D(disttex, st).bbb, 1.0);
}
