#version 150 core

uniform struct Spatial	{
	vec4 pos,rot;
}s_lit;
uniform vec4 lit_data, range_lit;

vec3 trans_inv(vec3,Spatial);
vec4 get_proj_lit(vec3);

in	vec3 at_base, at_pos;
out	float depth;


void main()	{
	float live = dot(at_base,at_base);
	gl_ClipDistance[0] = step(0.01,live)-0.5;

	vec3 p = trans_inv(at_pos, s_lit);
	depth = (p.z + range_lit.x) * range_lit.z;
	gl_Position = get_proj_lit(p);
}