// Procedural shading demo, Stefan Gustavson 2011.

uniform float time;
uniform sampler2D teximage;
varying vec2 dims;
varying vec2 one;
varying vec2 st;

void main( void )
{
  gl_FragColor = texture2D(teximage, st);
}
