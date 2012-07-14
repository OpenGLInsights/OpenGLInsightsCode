// Distance map contour texturing, Stefan Gustavson 2011
// A re-implementation of Green's method, with a 16-bit
// 8.8 distance map and explicit bilinear interpolation.
// This code is in the public domain.

uniform sampler2D disttex;
uniform float texw, texh;
varying float oneu, onev;
varying vec2 st;

// Replacement for RSL's 'filterstep()', with fwidth() done right.
// 'threshold ' is constant , 'value ' is smoothly varying
float aastep (float threshold , float value) {
  float afwidth = 0.7 * length ( vec2(dFdx(value), dFdy(value)));
  // GLSL 's fwidth(value) is abs(dFdx(value)) + abs(dFdy(value))
  return smoothstep (threshold-afwidth, threshold+afwidth, value );
}

void main( void )
{
  // Scale texcoords to range ([0,texw], [0,texh])
  vec2 uv = st * vec2(texw, texh);

  // Compute texel-local (u,v) coordinates for the four closest texels
  vec2 uv00 = floor(uv - vec2(0.5)); // Lower left corner of lower left texel
  vec2 uvlerp = uv - uv00 - vec2(0.5); // Texel-local lerp blends [0,1]

  // Perform explicit texture interpolation of distance value.
  // This is required for the split RG encoding of the 8.8 fixed-point value,
  // and as a bonus it works around the bad texture interpolation precision
  // in at least some ATI hardware.

  // Center st00 on lower left texel and rescale to [0,1] for texture lookup
  vec2 st00 = (uv00 + vec2(0.5)) * vec2(oneu, onev);

  // Compute distance value from four closest 8-bit RGBA texels
  vec4 D00 = texture2D(disttex, st00);
  vec4 D10 = texture2D(disttex, st00 + vec2(oneu, 0.0));
  vec4 D01 = texture2D(disttex, st00 + vec2(0.0, onev));
  vec4 D11 = texture2D(disttex, st00 + vec2(oneu, onev));

  // Restore the values for D from their 8.8 fixed point encoding
  vec2 D00_10 = vec2(D00.r, D10.r)*255.0-128.0 + vec2(D00.g, D10.g)*(255.0/256.0);
  vec2 D01_11 = vec2(D01.r, D11.r)*255.0-128.0 + vec2(D01.g, D11.g)*(255.0/256.0);

  // Interpolate along v
  vec2 D0_1 = mix(D00_10, D01_11, uvlerp.y);
  // Interpolate along u
  float D = mix(D0_1.x, D0_1.y, uvlerp.x);

  float g = aastep(0.0, D);
  // Final fragment color
  gl_FragColor = vec4(vec3(g), 1.0);
}
