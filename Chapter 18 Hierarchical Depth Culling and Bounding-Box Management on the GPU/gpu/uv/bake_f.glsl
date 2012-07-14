#version 150 core

in	vec4 to_vertex, to_quat;
out	vec4 re_vertex, re_quat;

void main()	{
	re_vertex = to_vertex;
	re_quat = to_quat;
}
