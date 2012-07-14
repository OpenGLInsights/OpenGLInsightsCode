#version 150 core

uniform struct Spatial	{
	vec4 pos,rot;
}s_lit,s_cam;
uniform vec4 range_lit;

vec3 trans_inv(vec3,Spatial);
vec4 get_proj_cam(vec3);

in vec4 at_vertex;
out vec4 lit_pos;

const float extent = 1.15;


void main()	{
	vec3 v = extent * range_lit.y * at_vertex.xyz + s_lit.pos.xyz;
	vec3 vc = trans_inv( v, s_cam );
	lit_pos.xyz = trans_inv( s_lit.pos.xyz, s_cam );
	lit_pos.w = s_cam.pos.w;	//cam->world scale
	gl_Position = get_proj_cam(vc);
}