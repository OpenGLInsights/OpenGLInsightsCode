#version 150 core

uniform struct Spatial	{
	vec4 pos,rot;
}s_cam;
uniform vec4 halo_data;


vec3 trans_inv(vec3,Spatial);
vec4 get_proj_cam(vec3);

	in	vec2 at_ghost_sys;
	in	vec3 at_ghost_pos;
	in	vec4 at_vertex;
flat	out	vec4 sl_pos,sl_rot;

const float extra = 1.5;


void main()	{
	if( at_ghost_sys.x >= 0.0 )	{
		sl_pos = vec4(at_ghost_pos,1.0);
		sl_rot = vec4(0.0);
		vec3 v = at_ghost_pos + extra * halo_data.x * at_vertex.xyz;
		vec3 vc = trans_inv(v,s_cam);
		gl_Position = get_proj_cam(vc);
	}else	gl_Position = vec4(0.0,0.0,-2.0,1.0);
}
