#version 420 core

uniform sampler2D		PositionTex;
uniform mat4			ViewMat;
uniform float			FarStart;
uniform float			FarEnd;
out vec4 				FragColor;

void main()
{
	vec4 p		= textureLod(PositionTex,gl_FragCoord.xy / vec2(textureSize(PositionTex,0)),0);
	float atInf = float(p.w==0.f);
	p.w			= 1.f;
	float depth = max(-(ViewMat * p).z,atInf*1000.f);
	float blur  = clamp( (depth-FarStart) / (FarEnd-FarStart), 0.01f, 1.f);
	FragColor   = vec4(blur,depth,1,1);
}

