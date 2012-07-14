#version 150 core

flat	in	float handness;
in	vec4	quaternion;

vec4 get_bump();
vec3 qrot2(vec4,vec3);

vec3 get_norm()	{
	vec3 bump = get_bump().xyz * vec3(handness,1.0,1.0);
	return  qrot2(normalize(quaternion), bump);
}
