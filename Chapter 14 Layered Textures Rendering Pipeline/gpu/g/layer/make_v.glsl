#version 150 core

in	vec4	at_vertex;
out	vec3	normal;


uniform struct Spatial	{
	vec4	pos,rot;
}s_model,s_view;

vec3 trans_for(vec3,Spatial);
vec4 get_proj_cam(vec3);
vec3 make_normal(vec4);


void main()	{
	normal = make_normal( s_model.rot );
	vec3 vc = trans_for( at_vertex.xyz, s_view );
	gl_Position = get_proj_cam(vc);
}
