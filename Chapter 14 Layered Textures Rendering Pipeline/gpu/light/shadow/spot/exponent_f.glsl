#version 150 core

uniform sampler2D unit_light;
uniform float k_dark, texel_offset;

const int NUM = 4;
const float off = sqrt(3.0);

const vec2 offsets[] = vec2[NUM](
	vec2(0.0), vec2(0.0, 2.0),
	vec2(-off,-1.0), vec2(+off,-1.0)
);
const float wes[] = float[NUM]( 0.4, 0.2, 0.2, 0.2 );

float get_accum(vec3 coord)	{
	//return texture(unit_light,coord).r;
	//float sample[NUM];
	float rez = 0.0;
	for(int i=0; i<NUM; ++i)	{
		vec2 tc = coord.xy + texel_offset * offsets[i];
		rez += wes[i] * texture(unit_light,tc).r;
	}
	return rez;
}

float get_shadow(vec4 coord)	{
	float occluder = get_accum(coord.xyz);
	float shadow = occluder * exp2( k_dark*(1.0-coord.z) );
	return clamp(shadow, 0.0, 1.0);
}
