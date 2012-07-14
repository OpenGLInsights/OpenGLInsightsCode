#version 150 core

uniform	vec4	proj_lit, lit_data;

vec4	project2(vec3,vec4);
float	get_shadow(vec4);


float comp_shadow(vec3 pl, float len)	{
	vec4 v_shadow = project2( pl, proj_lit );
	vec3 vlit = v_shadow.xyz / mix(1.0, v_shadow.w, lit_data.y);
	vec2 r2 = vlit.xy;
	float rad = smoothstep( 0.0, lit_data.x, 1.0-dot(r2,r2) );
	vec4 vs = vec4(0.5*vlit + vec3(0.5), 1.0);
	return rad * get_shadow(vs);
}