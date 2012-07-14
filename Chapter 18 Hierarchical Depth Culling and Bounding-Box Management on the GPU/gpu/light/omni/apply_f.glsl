#version 150 core

uniform vec4 lit_color, lit_data, proj_lit;
//uniform samplerCubeShadow unit_light;

float get_shadow()	{
	//return texture(unit_light, v_shadow);
	return 1.0;
}
vec4 get_lighting(vec3,vec3);


in vec3 v2lit, v2cam;
in vec4 v_shadow;
in float lit_int, lit_side;
out vec4 rez_color;


void main()	{
	vec3 v_lit = normalize(v2lit);
	vec3 v_cam = normalize(v2cam);
	//rez_color = vec4(1.0); return;
	
	float intensity = lit_int * get_shadow();
	if(intensity < 0.01) discard;
	
	rez_color = intensity*lit_color *
		get_lighting(v_lit,v_cam);
}
