#version 150 core

in	vec3	normal;

vec3 get_norm()	{
	return normalize(normal);
}