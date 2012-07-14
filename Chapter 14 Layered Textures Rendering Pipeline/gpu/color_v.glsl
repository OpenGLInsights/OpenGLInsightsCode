#version 150 core

in	vec4	at_vertex;
in	vec3	at_color0;
out	vec4	color;

uniform struct Spatial	{
	vec4	pos,rot;
}s_view;

vec3 trans_for(vec3,Spatial);
vec4 get_proj_cam(vec3);


void main()	{
	vec3 vc = trans_for( at_vertex.xyz, s_view );
	gl_Position = get_proj_cam(vc);
	color = vec4( at_color0/255.0, 0.0 );
}
