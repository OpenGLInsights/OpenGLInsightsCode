#version 150 core

in vec4 at_quat;

uniform struct Spatial	{
	vec4 pos,rot;
}s_model;

vec3 qrot(vec4,vec3);

vec3 dir_world(vec3 dir)	{
	vec3 v = qrot( at_quat, dir );
	return qrot( s_model.rot, v );
}
