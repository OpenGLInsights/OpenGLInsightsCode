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

// Cellular noise ("Worley noise") in 2D in GLSL.
// Copyright (c) Stefan Gustavson 2011-04-19. All rights reserved.
// This code is released under the conditions of the MIT license.
// See LICENSE file for details.

// Permutation polynomial: (34x^2 + x) mod 289
vec3 permute(vec3 x) {
  return mod((34.0 * x + 1.0) * x, 289.0);
}
// Cellular noise, returning F1 and F2 in a vec2.
// Standard 3x3 search window for good F1 and F2 values
vec2 cellular(vec2 P) {
#define K 0.142857142857 // 1/7
#define Ko 0.428571428571 // 3/7
#define jitter 1.0 // Less gives more regular pattern
	vec2 Pi = mod(floor(P), 289.0);
 	vec2 Pf = fract(P);
	vec3 oi = vec3(-1.0, 0.0, 1.0);
	vec3 of = vec3(-0.5, 0.5, 1.5);
	vec3 px = permute(Pi.x + oi);
	vec3 p = permute(px.x + Pi.y + oi); // p11, p12, p13
	vec3 ox = fract(p*K) - Ko;
	vec3 oy = mod(floor(p*K),7.0)*K - Ko;
	vec3 dx = Pf.x + 0.5 + jitter*ox;
	vec3 dy = Pf.y - of + jitter*oy;
	vec3 d1 = dx * dx + dy * dy; // d11, d12 and d13, squared
	p = permute(px.y + Pi.y + oi); // p21, p22, p23
	ox = fract(p*K) - Ko;
	oy = mod(floor(p*K),7.0)*K - Ko;
	dx = Pf.x - 0.5 + jitter*ox;
	dy = Pf.y - of + jitter*oy;
	vec3 d2 = dx * dx + dy * dy; // d21, d22 and d23, squared
	p = permute(px.z + Pi.y + oi); // p31, p32, p33
	ox = fract(p*K) - Ko;
	oy = mod(floor(p*K),7.0)*K - Ko;
	dx = Pf.x - 1.5 + jitter*ox;
	dy = Pf.y - of + jitter*oy;
	vec3 d3 = dx * dx + dy * dy; // d31, d32 and d33, squared
	// Sort out the two smallest distances (F1, F2)
	vec3 d1a = min(d1, d2);
	d2 = max(d1, d2); // Swap to keep candidates for F2
	d2 = min(d2, d3); // neither F1 nor F2 are now in d3
	d1 = min(d1a, d2); // F1 is now in d1
	d2 = max(d1a, d2); // Swap to keep candidates for F2
	d1.xy = (d1.x < d1.y) ? d1.xy : d1.yx; // Swap if smaller
	d1.xz = (d1.x < d1.z) ? d1.xz : d1.zx; // F1 is in d1.x
	d1.yz = min(d1.yz, d2.yz); // F2 is now not in d2.yz
	d1.y = min(d1.y, d1.z); // nor in  d1.z
	d1.y = min(d1.y, d2.x); // F2 is in d1.y, we're done.
	return sqrt(d1.xy);
}

// Cellular noise, returning F1 and F2 in a vec2 and the
// 2D vectors to each of the two closest points in a vec4.
// Standard 3x3 search window for good F1 and F2 values.
void cellular(in vec2 P, out vec2 F, out vec4 d1d2) {
#define K 0.142857142857 // 1/7
#define Ko 0.428571428571 // 3/7
#define jitter 1.0 // Less gives more regular pattern
	vec2 Pi = mod(floor(P), 289.0);
 	vec2 Pf = fract(P);
	vec3 oi = vec3(-1.0, 0.0, 1.0);
	vec3 of = vec3(-0.5, 0.5, 1.5);
	vec3 px = permute(Pi.x + oi);
	vec3 p = permute(px.x + Pi.y + oi); // p11, p12, p13
	vec3 ox = fract(p*K) - Ko;
	vec3 oy = mod(floor(p*K),7.0)*K - Ko;
	vec3 d1x = Pf.x + 0.5 + jitter*ox;
	vec3 d1y = Pf.y - of + jitter*oy;
	vec3 d1 = d1x * d1x + d1y * d1y; // d11, d12 and d13, squared
	p = permute(px.y + Pi.y + oi); // p21, p22, p23
	ox = fract(p*K) - Ko;
	oy = mod(floor(p*K),7.0)*K - Ko;
	vec3 d2x = Pf.x - 0.5 + jitter*ox;
	vec3 d2y = Pf.y - of + jitter*oy;
	vec3 d2 = d2x * d2x + d2y * d2y; // d21, d22 and d23, squared
	p = permute(px.z + Pi.y + oi); // p31, p32, p33
	ox = fract(p*K) - Ko;
	oy = mod(floor(p*K),7.0)*K - Ko;
	vec3 d3x = Pf.x - 1.5 + jitter*ox;
	vec3 d3y = Pf.y - of + jitter*oy;
	vec3 d3 = d3x * d3x + d3y * d3y; // d31, d32 and d33, squared
	// Sort out the two smallest distances (F1, F2)
	// While also swapping dx and dy accordingly
	vec3 comp3 = step(d2, d1);
	vec3 d1a = mix(d1, d2, comp3);
	vec3 d1xa = mix(d1x, d2x, comp3);
	vec3 d1ya = mix(d1y, d2y, comp3);
	d2 = mix(d2, d1, comp3); // Swap to keep candidates for F2
	d2x = mix(d2x, d1x, comp3);
	d2y = mix(d2y, d1y, comp3);

	comp3 = step(d3, d2);
	d2 = mix(d2, d3, comp3); // neither F1 nor F2 are now in d3
	d2x = mix(d2x, d3x, comp3);
	d2y = mix(d2y, d3y, comp3);

	comp3 = step(d2, d1a);
	d1 = mix(d1a, d2, comp3); // F1 is now in d1
	d1x = mix(d1xa, d2x, comp3);
	d1y = mix(d1ya, d2y, comp3);
	d2 = mix(d2, d1a, comp3); // Swap to keep candidates for F2
	d2x = mix(d2x, d1xa, comp3);
	d2y = mix(d2y, d1ya, comp3);

	float comp1 = step(d1.y, d1.x);
	d1.xy = mix(d1.xy, d1.yx, comp1); // Swap if smaller
	d1x.xy = mix(d1x.xy, d1x.yx, comp1);
	d1y.xy = mix(d1y.xy, d1y.yx, comp1);

	comp1 = step(d1.z, d1.x);
	d1.xz = mix(d1.xz, d1.zx, comp1); // F1 is in d1.x
	d1x.xz = mix(d1x.xz, d1x.zx, comp1);
	d1y.xz = mix(d1y.xz, d1y.zx, comp1);

	vec2 comp2 = step(d2.yz, d1.yz);
	d1.yz = mix(d1.yz, d2.yz, comp2); // F2 is now not in d2.yz
	d1x.yz = mix(d1x.yz, d2x.yz, comp2);
	d1y.yz = mix(d1y.yz, d2y.yz, comp2);

	comp1 = step(d1.z, d1.y);
	d1.y = mix(d1.y, d1.z, comp1); // nor in  d1.z
	d1x.y = mix(d1x.y, d1x.z, comp1);
	d1y.y = mix(d1y.y, d1y.z, comp1);

	comp1 = step(d2.x, d1.y);
	d1.y = mix(d1.y, d2.x, comp1); // F2 is in d1.y, we're done.
	d1x.y = mix(d1x.y, d2x.x, comp1);
	d1y.y = mix(d1y.y, d2y.x, comp1);
	F = sqrt(d1.xy);
	d1d2 = vec4(d1x.x, d1y.x, d1x.y, d1y.y);
}

void main( void )
{
  vec2 F;
  vec4 d1d2;
  cellular(8.0*st, F, d1d2); // Returns vectors to points
  // Constant width lines, from the book "Advanced RenderMan"
  float thresh = 0.05 * (length(d1d2.xy - d1d2.zw))
    / (F.x + F.y);
  float f = F.y - F.x;
  f += (0.5 - cellular(64.0*st).y) * thresh;
  gl_FragColor = vec4(aastep(thresh, f).xxx, 1.0);
}
