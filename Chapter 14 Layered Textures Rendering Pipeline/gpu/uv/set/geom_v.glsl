#version 150 core

out vec4 tog_vertex, tog_quat;

void set_surface(vec4 v, vec4 q)	{
	tog_vertex = v;
	tog_quat = q;
}