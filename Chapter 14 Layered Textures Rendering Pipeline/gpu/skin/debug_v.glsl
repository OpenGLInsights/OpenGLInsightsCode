#version 150 core

in	vec4	at_pos, at_rot, at_par;
out	vec4	pos, rot, par;

uniform struct Spatial	{
	vec4 pos, rot;
}s_cam;

vec3 trans_inv(vec3,Spatial);
vec4 get_proj_cam(vec3);


void main()	{
	pos = get_proj_cam( trans_inv(at_pos.xyz,s_cam) );
	rot = at_rot;
	par = get_proj_cam( trans_inv(at_par.xyz,s_cam) );
}