#version 150 core

uniform vec4 mat_diffuse;
uniform sampler1D unit_diffuse;

vec4 tc_diffuse();

vec4 get_diffuse()	{
	float tc = tc_diffuse().x;
	return mat_diffuse * texture(unit_diffuse,tc);
}
