#version 150 core

uniform	struct Spatial	{
	vec4 pos,rot;
}s_model,s_lit,s_cam;

uniform	vec4 range_lit;

//lib_quat
vec3	qrot(vec4,vec3);
vec4	qmul(vec4,vec4);
vec4	qinv(vec4);
vec3	trans_for(vec3,Spatial);
vec3	trans_inv(vec3,Spatial);
//lib_tool
float	get_attenuation(float);
vec4	get_proj_cam(vec3);
vec4	get_proj_lit(vec3);
float	get_proj_depth(float,vec4);
//mat
void	make_tex_coords();

in	vec4	at_vertex, at_quat;
out	vec3	v2lit, v2cam;
out	vec4	v_shadow;
out	float	lit_int, lit_depth;


void main()	{
	make_tex_coords();

	// vertex in world space
	vec3 v = trans_for(at_vertex.xyz, s_model);
	vec3 v_lit = s_lit.pos.xyz - v;
	vec3 v_cam = s_cam.pos.xyz - v;
	lit_int = get_attenuation( length(v_lit) );

	// vertex in camera space
	vec3 vc = trans_inv(v, s_cam);
	gl_Position = get_proj_cam(vc);
	
	// world -> tangent space transform
	vec3 hand = vec3(at_vertex.w, 1.0,1.0);
	vec4 quat = qinv(qmul( s_model.rot, at_quat ));
	v2lit = hand * qrot(quat, v_lit);
	v2cam = hand * qrot(quat, v_cam);
	
	// vertex in light space
	vec3 vl = trans_inv(v, s_lit);
	lit_depth = (range_lit.x + vl.z) * range_lit.z;
	v_shadow = get_proj_lit(vl);
}
