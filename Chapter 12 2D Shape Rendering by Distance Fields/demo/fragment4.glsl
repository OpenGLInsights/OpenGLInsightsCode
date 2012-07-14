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

  // Compute interpolated value from four closest 8-bit RGBA texels
  vec4 D00 = texture2D(disttex, st00);
  vec4 D10 = texture2D(disttex, st00 + vec2(oneu, 0.0));
  vec4 D01 = texture2D(disttex, st00 + vec2(0.0, onev));
  vec4 D11 = texture2D(disttex, st00 + vec2(oneu, onev));

  // Retrieve the B channel to get the original grayscale image
  vec4 G = vec4(D00.b, D01.b, D10.b, D11.b);
  
  // Interpolate along v
  G.xy = mix(G.xz, G.yw, uvlerp.y);
  
  // Interpolate along u
  float g = mix(G.x, G.y, uvlerp.x);

  float c = aastep(0.5, g);
  // Final fragment color
  gl_FragColor = vec4(vec3(c, c, c), 1.0);
}
