#version 150 core

uniform	vec4	range_lit;

float	get_shadow(vec4);


float comp_shadow(vec3 pl, float len)	{
	float nl = (len - range_lit.x) * range_lit.z;
	vec4 vs = vec4( pl.xyz, nl );
	return get_shadow(vs);
}