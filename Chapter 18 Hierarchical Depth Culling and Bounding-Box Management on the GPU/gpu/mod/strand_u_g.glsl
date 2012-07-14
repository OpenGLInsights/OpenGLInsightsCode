#version 150

uniform vec4 strand_data;

float get_shape(float k)	{
	return mix( 1.0+k, 1.0/(1.0-k), step(0.0,k) );
}
float get_fur_shape()	{
	return get_shape( strand_data.z );
}

// should be gentype here!

vec2 get_fur_strand(vec2 tc, vec3 par)	{
	float shape = get_shape( par.z );
	return mix( par.xx, par.yy, pow(tc,vec2(shape)) );
}
vec2 get_fur_thick(vec2 tc)	{
	return get_fur_strand( tc, strand_data.xyz );
}
vec2 get_fur_alpha(vec2 tc)	{
	return get_fur_strand( tc, vec3(1.0,0.0,strand_data.w) );
}

