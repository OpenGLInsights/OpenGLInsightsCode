#version 150 core

uniform float part_total;

float part_uni()	{
	return gl_VertexID * part_total;
}

float random(float seed)	{
	return fract(sin( 78.233*seed ) * 43758.5453);
}

vec3 random_cube(float seed)	{
	float w = seed * (1.0+gl_VertexID);
	vec3 off = vec3(0.121,1.351,3.415);
	vec3 v = (vec3(w) + off) * vec3(1.0,2.0,3.0);
	v = fract( sin(78.233*v ) * 43758.5453 );
	return v;	//no normalizing
}