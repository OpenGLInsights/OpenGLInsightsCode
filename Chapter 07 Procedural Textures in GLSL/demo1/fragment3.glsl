// Procedural shading demo, Stefan Gustavson 2011.

uniform float time;
uniform sampler2D teximage;
varying vec2 dims;
varying vec2 one;
varying vec2 st;

void main( void )
{
  float pattern = smoothstep(0.3, 0.32, length(fract(5.0*st)-0.5));
  gl_FragColor = vec4(pattern.xxx, 1.0);
}
