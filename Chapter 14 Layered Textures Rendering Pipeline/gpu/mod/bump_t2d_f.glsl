#version 150 core

uniform sampler2D unit_bump;

vec4 tc_bump();
vec3 fastnorm(vec3);	//lib_math

vec4 get_bump()	{
	vec2 tc = tc_bump().xy;
	vec4 bump = 2.0*texture(unit_bump,tc) - vec4(1.0);
	bump.y *= -1.0;	//is it correct for Blender?
	return vec4(fastnorm(bump.xyz), bump.w);
}
