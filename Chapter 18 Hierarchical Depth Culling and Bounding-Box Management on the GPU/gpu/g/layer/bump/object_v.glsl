#version 150 core

uniform struct Spatial	{
	vec4 pos,rot;
}s_model;

out	float handiness;

vec4	get_quaternion()	{
	handiness = 1.0;
	return s_model.rot;
}