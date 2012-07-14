#version 150 core

uniform vec4 fur_init;

out vec3 to_prev,to_base;

vec3 qrot(vec4,vec3);
vec3 random_cube(float);

const float seed	= 1.32442;


void set_base(vec4 vert, vec4 quat)	{
	if( dot(quat,quat)<2.0 )	{
		vec3 rnd = random_cube(seed);
		vec3 dir = fur_init.xyz + fur_init.w * rnd;
		dir.x *= vert.w;
		to_base = vert.xyz;
		to_prev = to_base - qrot(quat,dir);
	}else	{
		to_base = to_prev = vec3(0.0);
	}
}
