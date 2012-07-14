#version 150 core

uniform struct Spatial	{
	vec4	pos,rot;
}s_model,s_view;


//lib_quat
vec3 trans_for(vec3,Spatial);
//lib_tool
vec4 get_proj_cam(vec3);
//material
void make_tex_coords();
//deferred
void put_norm(vec4,float);

in	vec4	at_vertex;


void main()	{
	make_tex_coords();
	
	// tangent->world transform
	put_norm( s_model.rot, at_vertex.w );

	// vertex in camera space
	vec3 vc = trans_for(at_vertex.xyz, s_view);
	gl_Position = get_proj_cam(vc);
}
