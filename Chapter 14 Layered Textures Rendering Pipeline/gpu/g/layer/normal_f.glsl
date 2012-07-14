#version 150 core

uniform	sampler2D	unit_texture;

in	vec4	n_space;
in	float	handiness;

out	vec4	c_normal;

vec4	tc_unit();
vec3	qrot2(vec4,vec3);


void main()	{
	vec4	tc	= tc_unit();
	vec4	value	= texture( unit_texture, tc.xy );
	vec3	normal	= normalize(value.xyz*2.0 - vec3(1.0));
	vec4	quat	= normalize(n_space);
	vec3	n	= qrot2(quat,normal) * vec3(handiness,1.0,1.0);
	c_normal	= 0.5*vec4(n,0.0)+vec4(0.5);
}
