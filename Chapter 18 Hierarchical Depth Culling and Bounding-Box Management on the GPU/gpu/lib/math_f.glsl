#version 150 core

vec3 fastnorm(vec3 v)	{
	return v*(1.5 - 0.5*dot(v,v));
	//return normalize(v);
}

vec3 qrot2(vec4 q, vec3 v)	{
	return v + 2.0*cross(q.xyz, cross(q.xyz,v) + q.w*v);
}

vec4 qinv2(vec4 q)	{
	return vec4(-q.xyz,q.w);
}