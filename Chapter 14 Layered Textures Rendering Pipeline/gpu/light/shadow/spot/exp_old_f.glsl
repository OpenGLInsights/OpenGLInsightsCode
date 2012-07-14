#version 150 core

uniform sampler2D unit_light;
uniform vec4 k_dark;
in float lit_depth;

float log_conv(float x0, float X, float y0, float Y)	{
    return (X + log( x0 + (y0 * exp(Y-X)) ));
}
const vec2 offsets[] = vec2[9]( vec2(0.0),
	vec2(1.0,0.0), vec2(-1.0,0.0),
	vec2(0.0,1.0), vec2(0.0,-1.0),
	vec2(1.0,-1.0), vec2(-1.0,1.0),
	vec2(1.0,1.0), vec2(-1.0,-1.0)
);
const float wes[] = float[9]( 0.2,
	0.13, 0.13, 0.13, 0.13,
	0.07, 0.07, 0.07, 0.07
);

float get_accum(vec2 coord)	{
	//return texture(unit_light,coord).r;
	const int NUM = 9;
	float sample[NUM];
	vec2 scale = 1.0 / vec2( textureSize(unit_light,0) );
	for(int i=0; i<NUM; ++i)	{
		vec2 tc = coord + scale * offsets[i];
		sample[i] = texture(unit_light,tc).r;
	}
	float accum = log_conv(wes[0], sample[0], wes[1], sample[1]);
	for(int i=2; i<NUM; ++i)	{
		accum = log_conv(1.0, accum, wes[i], sample[i]);
	}
	return accum;
}

float get_shadow(vec4 coord)	{
	float occluder = get_accum(coord.xy);
	float receiver = lit_depth - k_dark.y;
	float shadow = exp(k_dark.x * (occluder - receiver));
	return clamp(shadow, 0.0, 1.0);
}
