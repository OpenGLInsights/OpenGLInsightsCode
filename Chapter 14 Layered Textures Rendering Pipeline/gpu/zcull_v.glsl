#version 150 core

uniform struct Spatial	{
	vec4 pos,rot;
}s_view;

in	vec4	at_vertex;

vec3 trans_for(vec3,Spatial);
vec4 get_proj_cam(vec3);


void main()	{
	vec3 vc = trans_for( at_vertex.xyz, s_view );
	gl_Position = get_proj_cam(vc);
}