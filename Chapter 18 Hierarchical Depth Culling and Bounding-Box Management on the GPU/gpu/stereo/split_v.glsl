#version 150 core

uniform	float		half_eye, focus_dist;

uniform struct Spatial	{
	vec4 pos,rot;
}s_view;


in	vec4	at_vertex;
out	vec4	pl,pc,pr;

vec3	trans_for(vec3,Spatial);
vec4	get_proj_cam(vec3);


void main()	{
	vec3 vc = trans_for( at_vertex.xyz, s_view );
	vec3 off = vec3(0.0);
	float div = focus_dist;	//alternative: -vc.z
	off.x = (vc.z+focus_dist) * half_eye / div;
	pc = get_proj_cam(vc);
	pl = get_proj_cam(vc-off);
	pr = get_proj_cam(vc+off);
}