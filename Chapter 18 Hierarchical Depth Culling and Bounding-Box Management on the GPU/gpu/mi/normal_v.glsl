#version 150 core

vec3 dir_world(vec3);

vec3 mi_normal()	{
	return dir_world( vec3(0.0,0.0,1.0) );
}
