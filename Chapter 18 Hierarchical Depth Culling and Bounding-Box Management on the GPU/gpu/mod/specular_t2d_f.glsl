#version 150 core

uniform vec4 mat_specular;
uniform sampler2D unit_specular;

vec4 tc_specular();

vec4 get_specular()	{
	vec2 tc = tc_specular().xy;
	return texture(unit_specular,tc);
}