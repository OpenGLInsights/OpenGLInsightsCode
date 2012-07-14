#version 150 core

const float k0 = 0.2;
const float k1 = sqrt(1.0 - k0*k0);

vec4 get_harmonics(vec3 dir)	{
	//expect normalized dir
	return vec4( k1*dir, k0 );
}
