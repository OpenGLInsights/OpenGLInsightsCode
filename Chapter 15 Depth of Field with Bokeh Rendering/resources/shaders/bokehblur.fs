#version 420 core

uniform sampler2D		BlurDepthTex;
uniform sampler2D		ColorTex;
uniform vec2			Direction;
uniform float			MaxCoCRadius;
out vec4 				FragColor;

void main()
{
	vec3 color				= texelFetch(ColorTex,ivec2(floor(gl_FragCoord.xy)),0).xyz;
	vec2 bd					= texelFetch(BlurDepthTex,ivec2(floor(gl_FragCoord.xy)),0).xy;
	float blur				= bd.x;
	float depth				= bd.y;
	float cocSize			= blur * MaxCoCRadius;
	vec3 outputColor		= vec3(0);

	if(cocSize>0)
	{
		int count			= 0;
		int nSamples		= int(ceil(MaxCoCRadius));
		float totalWeight	= 0;

		for(int i=-nSamples;i<=nSamples;++i)
		{
			vec2 coord		= floor(gl_FragCoord.xy) + i*Direction;
			vec2 blurDepth	= texelFetch(BlurDepthTex,ivec2(coord),0).xy;

			float cocWeight = clamp(cocSize + 1.0f - abs(float(i)),0,1);
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
