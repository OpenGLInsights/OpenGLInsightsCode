#version 150 core

uniform struct Spatial	{
	vec4 pos,rot;
}s_model;

Spatial get_surface(vec2 tc)	{
	return s_model;
}