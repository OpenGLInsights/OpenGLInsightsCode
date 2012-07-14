#version 150 core

uniform vec4 part_life;

vec4 part_time();
float part_uni();


bool born_ready()	{
	vec4 t = part_time();
	float u = part_uni();
	return t.z > u*part_life.z;
}
