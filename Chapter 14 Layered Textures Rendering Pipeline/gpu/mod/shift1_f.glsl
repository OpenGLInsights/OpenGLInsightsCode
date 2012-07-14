#version 150 core

vec4 mat_bump(vec4);
uniform vec4 mat_scalars;

vec4 mat_shift(vec4 tex, vec3 view)	{
	vec4 b = mat_bump(tex);
	return tex + mat_scalars.w * b.z*b.w * vec4(view.xy,0.0,0.0);
}
