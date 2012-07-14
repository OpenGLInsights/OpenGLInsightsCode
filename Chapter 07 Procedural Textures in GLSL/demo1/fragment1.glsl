// Procedural shading demo, Stefan Gustavson 2011.

uniform float time;
uniform sampler2D teximage;
varying vec2 dims;
varying vec2 one;
varying vec2 st;

void main( void )
{
  gl_FragColor = vec4(1.0, 0.5, 0.0, 1.0);
}
