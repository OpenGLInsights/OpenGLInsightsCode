#version 150 core

vec4 get_diffuse();
vec4 get_specular();
float get_glossiness();

float comp_diffuse(vec3,vec3);
float comp_specular(vec3,vec3,vec3,float);

//normed version

vec4 get_lighting(vec3 no, vec3 lit, vec3 cam)	{
	float glossy = get_glossiness();
	return	comp_diffuse(no,lit) * get_diffuse() +
		comp_specular(no,lit,cam,glossy) * get_specular();
}

//bumped version
vec4 get_bump();

vec4 get_lighting(vec3 lit, vec3 cam)	{
	vec3 no = get_bump().xyz;
	return	get_lighting(no,lit,cam);
}

//parallaxed version
void apply_tex_offset(vec3);

vec4 get_lighting(float kp, vec3 lit, vec3 cam)	{
	vec4 b = get_bump();
	vec3 off = (kp * b.z * b.w) * cam;
	apply_tex_offset(off);
	return get_lighting(lit,cam);
}
