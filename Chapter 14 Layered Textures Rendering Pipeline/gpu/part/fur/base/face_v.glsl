#version 150 core
//#define INT
//#define FILTER

uniform sampler2D unit_vert, unit_quat;
uniform int width;

void set_base(vec4,vec4);
vec3 random_cube(float);

const float factor_jitter	= 0.9;
const float kuv			= 2.0;
const float seed		= 2.3432;

vec2 rand_off()	{
	return mix( vec2(0.5), random_cube(seed).xy, factor_jitter );
}


void main()	{
	#ifdef INT
	int yc = gl_VertexID / width;
	ivec2 tc = ivec2(gl_VertexID - yc*width, yc);
	vec4 vert = texelFetch(unit_vert,tc,0);
	vec4 quat = texelFetch(unit_quat,tc,0);
	#else
	int cy = gl_VertexID / width,
		cx = gl_VertexID - cy*width;
	vec2 tc = vec2(cx,cy);
	float dw = 1.0 / width;
	vec2 t0 = dw * (tc + vec2(0.5));
	vec4 vert = texture(unit_vert,t0);
	#ifdef FILTER
	vec2 t1 = dw * (tc + rand_off());
	vec4 v0=vert, v1=texture(unit_vert,t1);
	float diff = v0.w - v1.w;
	float param = step(0.1,diff*diff);
	// alternative stretch-estimation method
	//float diff = dot(v1-v0,v1-v0) - kuv * dot(t1-t0,t1-t0) * abs(v0.w);
	//float param = step(0.0, diff);
	vert = mix(v1,v0,param);
	vert.w = sign( v0.w );
	#endif
	vec4 quat = texture(unit_quat,t0);
	#endif
	set_base( vert, 2.0*quat-vec4(1.0) );
}
