#version 150 core

uniform struct Spatial	{
	vec4 pos,rot;
}s_model;

in	vec4	at_quat;

vec4	qmul(vec4,vec4);


vec4	get_quaternion()	{
	return qmul( s_model.rot, at_quat );
}