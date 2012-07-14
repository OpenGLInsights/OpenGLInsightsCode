#version 150 core

uniform struct Spatial	{
	vec4 pos,rot;
}s_model,s_cam;


//tools
vec3	qrot(vec4,vec3);
vec4	qmul(vec4,vec4);
vec3	trans_for(vec3,Spatial);
vec3	trans_inv(vec3,Spatial);
vec4	get_proj_cam(vec3);
//externals
vec4	get_quaternion();
void	make_tex_coords();

in	vec4	at_vertex;
in	vec4	at_quat;
in	vec3	at_normal;
//bump mapping
out	vec4	n_space;
out	float	handiness;
//parallax
out	vec3	view;
out	vec4	var_quat;
out	vec3	var_normal;


void main()	{
	make_tex_coords();
	handiness = at_vertex.w;
	n_space = get_quaternion();

	vec3 vw = trans_for( at_vertex.xyz, s_model );
	vec3 vc = trans_inv( vw, s_cam );
	gl_Position = get_proj_cam(vc);

	//world space normal, quaternion and camera vector
	var_quat	= qmul( s_model.rot, at_quat );
	var_normal	= qrot( s_model.rot, at_normal );
	view		= s_cam.pos.xyz - vw;
}
