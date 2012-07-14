#version 150 core

in	vec4	at_vertex;

uniform struct Spatial	{
	vec4 pos,rot;
}s_view;

void make_tex_coords();
vec3 trans_for(vec3,Spatial);
vec4 get_proj_cam(vec3);


void main()	{
	make_tex_coords();
	vec3 vc = trans_for( at_vertex.xyz, s_view );
	gl_Position = get_proj_cam(vc); 
}
