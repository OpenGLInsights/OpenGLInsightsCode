#version 150 core

uniform samplerCubeShadow unit_light;

float get_shadow(vec4 coord)	{
	return texture(unit_light, coord);
}