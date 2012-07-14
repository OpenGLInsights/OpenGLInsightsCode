#version 150 core

in vec4 at_vertex;
out vec4 to_old, to_new;


uniform struct Spatial	{
	vec4 pos,rot;
}s_view,s_offset;
uniform vec4 proj_cam;


vec3 trans_for(vec3,Spatial);
vec3 trans_inv(vec3,Spatial);
Spatial trans_combine(Spatial,Spatial);
vec4 get_proj_cam(vec3);


Spatial skin_append_all();


void main()	{
	Spatial sof = trans_combine( s_offset, skin_append_all() );
	
	vec3 vn = trans_for( at_vertex.xyz, s_view );
	vec3 vo = trans_inv( vn, s_offset );

	to_old = get_proj_cam(vo);
	to_new = get_proj_cam(vn);
	gl_Position = to_new;
}
