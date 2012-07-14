#version 150 core

in	vec3	at_normal;
out	vec3	normal;

vec3 qrot(vec4,vec3);

void put_norm(vec4 rot, float w)	{
	normal = qrot( rot, at_normal );
}