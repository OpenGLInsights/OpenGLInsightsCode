#version 150 core

uniform vec4 screen_size, lit_color, proj_cam;
uniform sampler2D unit_depth;

vec3 unproject(vec3,vec4);
float get_attenuation2(float);

in vec4 lit_pos;	//cam space light position
out vec3 rez_dir;	//cam space normalized direction to the light
out vec4 rez_color;	//light color with contribution applied

const float threshold = 0.01;


void main()	{
	//rez_color = vec4(1.0); rez_dir = vec3(1.0); return;

	// extract world & light space
	vec2 tc = gl_FragCoord.xy / screen_size.xy;
	float depth = texture(unit_depth,tc).r;
	vec3 p_camera = unproject( vec3(tc,depth), proj_cam );
	
	// compute components
	vec3 dir = lit_pos.w * (lit_pos.xyz - p_camera);
	float len = length( dir );
	float intensity = clamp( get_attenuation2(len), 0.0,1.0 );

	// translate to output
	rez_color = intensity * lit_color;
	vec3 col = rez_color.xyz;
	if( dot(col,col) < threshold ) discard;
	rez_dir = dir / len;	//normalized
}
