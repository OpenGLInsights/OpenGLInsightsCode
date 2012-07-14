#version 150 core

//---	UNIFORMS	---//

uniform sampler2D	unit_depth;
uniform sampler2D	unit_g2;	//no need for diffuse/specular
uniform samplerCube	unit_env;

uniform struct Spatial	{
	vec4	pos,rot;
}s_cam,s_model;

uniform vec4	screen_size, proj_cam, env_multi;


//---	TRANSFORM	---//
vec3	trans_for2(vec3,Spatial);
vec3	unproject(vec3,vec4);

//---	VARYINGS	---//
out	vec4	rez_color;


//---	MAIN	---//

void main()	{
	vec2 tc = gl_FragCoord.xy / screen_size.xy;
	
	//extract world coordinate
	float depth	= texture(unit_depth,tc).r;
	vec3 p_camera	= unproject( vec3(tc,depth), proj_cam );
	vec3 p_world	= trans_for2(p_camera, s_cam);

	//extract world normal
	vec4 g_normal	= texture(unit_g2,tc);
	vec3 normal	= 2.0*g_normal.xyz - vec3(1.0);	//world space
	
	//compute environmental contribution
	vec3 v_cam	= p_world - s_cam.pos.xyz;
	vec3 reflected	= reflect(v_cam,normal);
	rez_color	= env_multi * texture(unit_env,reflected);
}