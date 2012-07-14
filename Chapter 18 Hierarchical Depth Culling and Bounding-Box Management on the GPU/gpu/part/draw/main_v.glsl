#version 150 core

uniform struct Spatial	{
	vec4 pos,rot;
}s_cam;
uniform vec4 proj_cam, screen_size;


vec3 trans_inv(vec3,Spatial);
vec4 get_proj_cam(vec3);

float get_project_scale(vec4 pos)	{
	float ortho = step( 0.0, proj_cam.w ) ;
	float kpers = 1.0 / (s_cam.pos.w * pos.w);
	return screen_size.z * mix( kpers, proj_cam.x, ortho );
}

void part_draw(vec3 pos, float size)	{
	gl_Position = get_proj_cam( trans_inv(pos, s_cam) );
	gl_PointSize = 2.0*size * get_project_scale( gl_Position );
}