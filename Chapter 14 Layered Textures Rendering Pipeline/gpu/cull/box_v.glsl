#version 150 core

in	vec4 at_vertex;
out	vec3 position;

void main()	{
	position = at_vertex.xyz;
}
