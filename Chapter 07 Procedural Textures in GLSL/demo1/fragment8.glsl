// Procedural shading demo, Stefan Gustavson 2011.

uniform float time;
uniform sampler2D teximage;
varying vec2 dims;
varying vec2 one;
varying vec2 st;

float aastep ( float threshold , float value ) {
  float afwidth = 0.7 * length ( vec2 ( dFdx ( value ), dFdy ( value )));
  return smoothstep ( threshold - afwidth , threshold + afwidth , value );
}

// Explicit bilinear texture lookup to circumvent bad hardware precision.
// The extra arguments specify the dimension of the texture. (GLSL 1.30
// introduced textureSize() to get that information from the sampler.)
// 'dims' is the width and height of the texture, 'one' is 1.0/dims.
// (Precomputing 'one' saves two divisions for each lookup.)
vec4 texture2D_bilinear(sampler2D tex, vec2 st, vec2 dims, vec2 one) {
  vec2 uv = st * dims;
  vec2 uv00 = floor(uv - vec2(0.5)); // Lower left corner of lower left texel
  vec2 uvlerp = uv - uv00 - vec2(0.5); // Texel-local lerp blends [0,1]
  vec2 st00 = (uv00 + vec2(0.5)) * one;
  vec4 texel00 = texture2D(tex, st00);
  vec4 texel10 = texture2D(tex, st00 + vec2(one.x, 0.0));
  vec4 texel01 = texture2D(tex, st00 + vec2(0.0, one.y));
  vec4 texel11 = texture2D(tex, st00 + one);
  vec4 texel0 = mix(texel00, texel01, uvlerp.y); 
  vec4 texel1 = mix(texel10, texel11, uvlerp.y); 
  return mix(texel0, texel1, uvlerp.x);
}

// Description : Array- and textureless GLSL 2D simplex noise.
// Author : Ian McEwan, Ashima Arts. Version: 20110822
// Copyright (C) 2011 Ashima Arts. All rights reserved.
// Distributed under the MIT License. See LICENSE file.
// https://github.com/ashima/webgl-noise

vec2 mod289(vec2 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec3 mod289(vec3 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec3 permute(vec3 x) { return mod289(((x*34.0)+1.0)*x); }

float snoise(vec2 v) {
  const vec4 C = vec4(0.211324865405187,  // (3.0-sqrt(3.0))/6.0
                      0.366025403784439,  // 0.5*(sqrt(3.0)-1.0)
                     -0.577350269189626,  // -1.0 + 2.0 * C.x
                      0.024390243902439); // 1.0 / 41.0
// First corner
  vec2 i = floor(v + dot(v, C.yy) );
  vec2 x0 = v - i + dot(i, C.xx);
// Other corners
  vec2 i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
  vec4 x12 = x0.xyxy + C.xxzz;
  x12.xy -= i1;
// Permutations
  i = mod289(i); // Avoid truncation effects in permutation
  vec3 p = permute( permute( i.y + vec3(0.0, i1.y, 1.0 ))
                           + i.x + vec3(0.0, i1.x, 1.0 ));
  vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy),
                          dot(x12.zw,x12.zw)), 0.0);
  m = m*m; m = m*m;
// Gradients
  vec3 x = 2.0 * fract(p * C.www) - 1.0;
  vec3 h = abs(x) - 0.5;
  vec3 a0 = x - floor(x + 0.5);
// Normalise gradients implicitly by scaling m
  m *= 1.792843 - 0.853735 * ( a0*a0 + h*h );
// Compute final noise value at P
  vec3 g;
  g.x = a0.x * x0.x + h.x * x0.y;
  g.yz = a0.yz * x12.xz + h.yz * x12.yw;
  return 130.0 * dot(m, g);
}

void main( void )
{
  vec3 rgb = texture2D_bilinear(teximage, st, dims, one).rgb;
  //vec3 rgb = texture2D(teximage, st).rgb; // RGB image
  float n = 0.1*snoise(st*200.0); // Fractal noise
  n += 0.05*snoise(st*400.0);
  n += 0.025*snoise(st*800.0);
  vec4 cmyk;
  cmyk.xyz = 1.0 - rgb; // Rough CMY conversion
  cmyk.w = min(cmyk.x, min(cmyk.y, cmyk.z)); // Create K
  cmyk.xyz -= cmyk.w; // Subtract K equivalent from CMY

  vec2 Kst = 50.0*mat2(0.707, -0.707, 0.707, 0.707)*st;
  vec2 Kuv = fract(Kst)-0.5;
  float k = aastep(0.0, sqrt(cmyk.w)-2.0*length(Kuv)+n);
  vec2 Cst = 50.0*mat2(0.966, -0.259, 0.259, 0.966)*st;
  vec2 Cuv = fract(Cst)-0.5;
  float c = aastep(0.0, sqrt(cmyk.x)-2.0*length(Cuv)+n);
  vec2 Mst = 50.0*mat2(0.966, 0.259, -0.259, 0.966)*st;
  vec2 Muv = fract(Mst)-0.5;
  float m = aastep(0.0, sqrt(cmyk.y)-2.0*length(Muv)+n);
  vec2 Yst = 50.0*st; // 0 deg
  vec2 Yuv = fract(Yst)-0.5;
  float y = aastep(0.0, sqrt(cmyk.z)-2.0*length(Yuv)+n);

  vec3 rgbscreen = 1.0-vec3(c,m,y);
  rgbscreen = mix(rgbscreen, vec3(0.0), 0.7*k + 0.5*n);
  vec2 fw = fwidth(st);
  float blend = smoothstep(0.7, 1.4, 200.0*max(fw.s, fw.t));
  gl_FragColor = vec4(mix(rgbscreen, rgb, blend), 1.0);
}
