#version 420 core

uniform sampler2D		BlurDepthTex;
uniform sampler2D		ColorTex;
uniform sampler2D		RotationTex;
uniform int				NSamples;
uniform float			MaxCoCRadius;
uniform vec2			Samples[32];
out vec4 				FragColor;

void main()
{
    ivec2 pix               = ivec2(floor(gl_FragCoord.xy));
	vec3 color				= texelFetch(ColorTex,pix,0).xyz;
	vec2 bd					= texelFetch(BlurDepthTex,pix,0).xy;
	float blur				= bd.x;
	float depth				= bd.y;
	float cocSize			= blur * MaxCoCRadius;
	vec3 outputColor		= vec3(0);
    vec2 theta	            = texelFetch(RotationTex,pix,0).xy;
    mat2 rot 	            = mat2(theta.x,theta.y,-theta.y,theta.x);
	if(cocSize>0)
	{
		int count			= 0;
		float totalWeight	= 0;
		for(int i=0;i<NSamples;++i)
		{
			float neighDist = length(Samples[i])*cocSize;
			vec2 coord		= floor(gl_FragCoord.xy) + (rot * Samples[i])*cocSize;
			vec2 blurDepth	= texelFetch(BlurDepthTex,ivec2(coord),0).xy;
			float cocWeight = clamp(cocSize + 1.0f - neighDist,0,1);

			float depthWeight= float(blurDepth.y >= depth);
			float blurWeight= blurDepth.x;
			float tapWeight = cocWeight * clamp(depthWeight + blurWeight,0,1);
			vec3 color		= texelFetch(ColorTex,ivec2(coord),0).xyz;
			
			outputColor		+= color*tapWeight;
			totalWeight		+= tapWeight;
		}
		outputColor /= totalWeight;
	}
	else
	{
		outputColor = color;
	}

	FragColor		 = vec4(outputColor,1);
}

