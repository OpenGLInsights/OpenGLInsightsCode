#version 150 core

uniform vec4 mat_diffuse;
uniform sampler2D unit_diffuse;

vec4 tc_diffuse();

vec4 get_diffuse()	{
	vec2 tc = tc_diffuse().xy;
	return mat_diffuse * texture(unit_diffuse,tc);
}
