#version 150 core

uniform vec4 mat_bump;
vec4 get_bump();
in vec4 tc_diffuse,tc_bump;

void make_shift(vec3 view)	{
	vec4 b = get_bump();
	vec4 off = mat_bump.w * b.z*b.w * vec4(view.xy,0.0,0.0);
	tc_diffuse += off;
	tc_bump += off;
}
