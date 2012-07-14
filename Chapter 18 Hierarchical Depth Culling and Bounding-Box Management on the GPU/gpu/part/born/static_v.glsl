#version 150 core

uniform vec4 part_life;

vec4 part_time();

bool born_ready()	{
	vec4 t = part_time();
	return t.w == 0.0;
}
