#version 420 core

uniform sampler2D		BokehShapeTex;
uniform sampler2D		BlurDepthTex;
uniform float			BokehDepthCutoff;
in  vec4				gColor;
in  float				gDepth;
in  vec2				gTexCoord;
out vec4 				FragColor;

void main()
{
	float alpha	= textureLod(BokehShapeTex,gTexCoord,0).x;
	vec2  bd	= textureLod(BlurDepthTex,gl_FragCoord.xy/vec2(textureSize(BlurDepthTex,0)),0).xy;
	float blur  = bd.x;
	float depth = bd.y;

	// Depth test for avoiding bokeh overlapping above on-focused objects
	float weight= clamp(depth - gDepth + BokehDepthCutoff,0,1);
	weight		= clamp(weight + blur,0,1);

	FragColor	= vec4(gColor.xyz * alpha * weight,gColor.w);

	// For debugging/displaying bokehs
//	if(gl_FragCoord.x<10000)
//	{
//		FragColor	= vec4(0,0,15000,1);
//	}
}

