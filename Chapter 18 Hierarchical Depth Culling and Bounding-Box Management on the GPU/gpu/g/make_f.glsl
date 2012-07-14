#version 150 core

//material
vec4	get_bump();
vec4	get_diffuse();
float	get_emissive();
vec4	get_specular();
float	get_glossiness();
//deferrred
vec3 get_norm();

flat	in	float handness;
in	vec4	quat;
in	vec4	coord_text, coord_bump;

out	vec4	c_diffuse;
out	vec4	c_specular;
out	vec4	c_normal;


void main()	{
	vec3 w_norm = get_norm();
	float glossy = get_glossiness() / 256.0;
	vec3 diff = get_diffuse().xyz;
	
	c_diffuse	= vec4( diff, get_emissive() );
	c_specular	= vec4( get_specular().xyz, glossy );
	c_normal	= vec4(vec3(0.5) + 0.5*w_norm, 0.0);
}
