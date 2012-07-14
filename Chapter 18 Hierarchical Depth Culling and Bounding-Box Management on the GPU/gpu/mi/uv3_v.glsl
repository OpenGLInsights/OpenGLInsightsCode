#version 150 core

in vec2 at_tex3;

vec3 mi_uv3()	{
	return vec3(at_tex3,0.0);
}
