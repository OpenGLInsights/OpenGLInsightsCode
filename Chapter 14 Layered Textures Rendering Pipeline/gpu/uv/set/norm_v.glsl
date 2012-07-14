#version 150 core

out vec4 to_vertex, to_quat;

void set_surface(vec4 v, vec4 q)	{
	to_vertex = v;
	to_quat = q;
}