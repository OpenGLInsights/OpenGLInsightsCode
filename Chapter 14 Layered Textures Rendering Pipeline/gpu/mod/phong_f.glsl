#version 150 core

float comp_specular(vec3 no, vec3 lit, vec3 cam, float glossy)	{
	vec3 ha = normalize(lit+cam);	//half-vector
	float nh = max( dot(no,ha), 0.0);
	return pow(nh,glossy);
}