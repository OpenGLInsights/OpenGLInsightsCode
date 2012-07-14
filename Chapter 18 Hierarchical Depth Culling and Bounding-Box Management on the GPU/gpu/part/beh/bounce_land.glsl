#version 150 core

out	vec3 to_pos, to_speed;

uniform sampler2D unit_land;

uniform struct Spatial	{
	vec4 pos,rot;
}s_land;

uniform vec4 proj_land;


const vec3 bounce = vec3(0.95,0.95,-0.3);

vec4 qinv(vec4);
vec3 qrot(vec4,vec3);
vec3 trans_inv(vec3,Spatial);
vec4 get_projection(vec3,vec4);


float update_land()	{
	vec3 pos = trans_inv(to_pos,s_land);
	vec3 spd = qrot( qinv(s_land.rot), to_speed );
	vec4 proj = 0.5*get_projection(pos,proj_land) + vec4(0.5);
	float depth = textureProj(unit_land, proj.xyw).x;
	if(depth*proj.w < proj.z && spd.z<0.0)	{
		to_speed = qrot( s_land.rot, spd*bounce );
	}
	return 1.0;
}
