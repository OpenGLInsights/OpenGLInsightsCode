#version 150 core

uniform struct Spatial	{
	vec4 pos,rot;
}s_view;

vec3 trans_for(vec3,Spatial);
vec4 get_proj_cam(vec3);


void skin_finish(vec3 pos)	{
	vec3 vc = trans_for(pos,s_view);
	gl_Position = get_proj_cam(vc);
}
