#version 150 core
#define NORMAL

uniform vec4 lit_color, lit_data;

vec3 qrot2(vec4,vec3);
vec4 qinv2(vec4);
float get_shadow(vec4);
vec4 get_diffuse();
vec4 get_specular();
float get_glossiness();

in vec3 surf_norm, dir_light, dir_view;
in vec4 color, v_shadow, fur_quat;
in float fur_x;
out vec4 rez_color;


void apply_shadow()	{
	//return;
	vec3 vs = v_shadow.xyz * (0.5/v_shadow.w) + vec3(0.5);
	rez_color *= get_shadow( vec4(vs,1.0) );
	//rez_color = vec3( get_shadow( vec4(vs,1.0) ));
}


void main()	{
	vec4 quat = normalize(fur_quat);
	vec3 L = normalize(dir_light);
	vec3 V = normalize(dir_view);
	vec3 T = qrot2(quat,vec3(0.0,0.0,1.0));

	#ifdef NORMAL
	vec3 N = qrot2(quat,vec3(1.0,0.0,0.0));
	float pr_diff = dot(N,L), pr_spec =	
		dot(V, 2.0*pr_diff*N - L);
	vec2 par = max( vec2(0.0001), vec2(pr_diff,pr_spec) );
	float diffuse = par.x;
	diffuse = 0.5*(1.0 + pr_diff);
	float specular = pow( par.y, get_glossiness() );
	#else
	float t_lit = dot(L,T), t_view = dot(V,T);
	vec3 n_lit = L - t_lit*T, n_view = V-t_view*T;

	float diffuse = max( 0.0, dot(L,n_lit) );
	float sq = dot(n_view,n_view) * dot(n_lit,n_lit);
	float pr_spec = sqrt(sq) - t_lit*t_view;
	float specular = pow( max(0.01,pr_spec), get_glossiness() );
	#endif

	//rez_color = vec4( normalize(fur_tan), color.w );
	rez_color = lit_color * (diffuse * get_diffuse() + specular * get_specular());
	apply_shadow();	
	rez_color.w = color.w * (1.0 - fur_x*fur_x);
}