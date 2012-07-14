#version 150 core

uniform	float mat_emissive;
uniform sampler2D unit_emissive;

vec4 tc_emissive();

float get_emissive()	{
	vec2 tc = tc_emissive().xy;
	return mat_emissive * texture(unit_emissive,tc).x;
}
