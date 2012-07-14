#version 150 core

uniform vec4 screen_size, lit_color, proj_cam;
uniform sampler2D unit_depth;

vec3 unproject(vec3,vec4);
float get_attenuation2(float);
vec4 get_harmonics(vec3);

in vec4 lit_pos;	//cam space!
out vec4 ca,cb,cc;


uniform vec4 lit_attenu;

void main()	{
	// extract world & light space
	vec2 tc = gl_FragCoord.xy / screen_size.xy;
	float depth = texture(unit_depth,tc).r;
	vec3 p_camera = unproject( vec3(tc,depth), proj_cam );
	
	// compute components
	vec3 dir = lit_pos.w * (lit_pos.xyz - p_camera);
	float len = length( dir );
	vec4 kf = get_harmonics( dir/len );
	float intensity = max(0.0, get_attenuation2(len) );
	vec3 color = intensity * lit_color.xyz;

	// translate to output
	ca = color.r * kf;
	cb = color.g * kf;
	cc = color.b * kf;
}