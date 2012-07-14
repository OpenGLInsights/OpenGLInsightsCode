// DEPRECATED!
// leftover from kri 2-nd iteration
// pending adoption to the current iter

#extension GL_ARB_texture_rectangle : require
uniform sampler2DRect texture,tex_depth;
uniform float focal,inv_radius;

#define NUM_TAPS 12
const vec2 poisson[] = vec2[NUM_TAPS](
	vec2(-0.3262, -0.4058),
	vec2(-0.8401, -0.0735),
	vec2(-0.6959, +0.4571),
	vec2(-0.2033, +0.6207),
	vec2(+0.9623, -0.1949),
	vec2(+0.4734, -0.4800),
	vec2(+0.5194, +0.7670),
	vec2(+0.1854, -0.8931),
	vec2(+0.5074, +0.0644),
	vec2(+0.8964, +0.4124),
	vec2(-0.3219, -0.9326),
	vec2(-0.7915, -0.5977)
);

float mod_depth(float depth)	{
	//return depth;
	//return pow(depth,50.0);
	//return 1.0 + 1.0/log( (1.0-depth)*exp(-1.0) );
	return 1.0 - sqrt(1.0 - depth*depth);
	//const float st = 3.0; return 1.0 - pow(1.0 - pow(depth,st), 1.0/st);
	//return 1.0+log(depth);
}

void main()	{
	vec2 texel = gl_TexCoord[0].st;
	float depth = mod_depth( texture2DRect(tex_depth, texel).r );
	//gl_FragColor = texture2DRect(tex_depth, texel); return;
	//gl_FragColor = vec4(depth); return;
	//float amount = 1.0 + min(0.f, 2.75*(radius - abs(depth-focal)) );
	float amount = abs(depth-focal) * inv_radius;
	if(amount > 1.0)	{
		gl_FragColor = vec4(0.0);
		for(int i=0; i!=NUM_TAPS; ++i)	{
			vec2 tex = texel + amount * poisson[i];
			gl_FragColor += texture2DRect(texture,tex);
		}
		gl_FragColor *= 1.0 / float(NUM_TAPS);
	}else gl_FragColor = texture2DRect(texture, texel);
}
