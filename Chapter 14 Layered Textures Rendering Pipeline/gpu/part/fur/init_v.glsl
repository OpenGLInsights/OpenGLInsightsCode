#version 150 core

in	vec3 at_base;
out	vec3 to_pos, to_speed;

void init();

void main()	{
	to_pos = at_base;
	to_speed = vec3(0.0);
	init();
}
