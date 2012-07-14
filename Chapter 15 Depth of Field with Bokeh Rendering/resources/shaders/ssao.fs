#version 420 core

#ifdef SSAO_PASS
	uniform sampler2D		PositionTex;
	uniform sampler2D		NormalTex;
	uniform sampler2D		RotationTex;

	uniform float			Near;
	uniform mat4			View;
	uniform float			Beta;
	uniform float			Epsilon;
	uniform float			Kappa;
	uniform float			Sigma;
	uniform float			Radius;
	uniform int				nSamples;
	uniform vec2			Samples[32];

	out vec4 				FragColor;

	void main()
	{
		vec2 pix	= gl_FragCoord.xy / vec2(textureSize(PositionTex,0));
		vec2 theta	= texture(RotationTex,pix).xy;
		vec4 c		= texture(PositionTex,pix);
		vec3 vn		= normalize( (View * vec4(texture(NormalTex,pix).xyz,0)).xyz );
	 	vec3 vc		= (View * c).xyz;
		float r 	= Radius * abs(Near/vc.z);
		float A 	= 0;
		mat2 rot 	= mat2(theta.x,theta.y,-theta.y,theta.x);

		for(int i=0;i<nSamples;++i)
		{
			vec2 samp	= pix + (rot*Samples[i])*r;
			vec4 p		= texture(PositionTex,samp);
			vec3 v		= (View * p).xyz - vc;
			A 			+= max(0.f,dot(v,vn) + v.z*Beta)  / (dot(v,v) + Epsilon);
		}

		A 			= pow(max(0.f,1.f - 2.f*Sigma/float(nSamples)*A),Kappa);
		FragColor 	= vec4(A,A,A,A);
	}
#endif

#ifdef BILATERAL_PASS
	uniform sampler2D		InputTex;
	uniform sampler2D		PositionTex;
	
	uniform vec2			Direction;
	uniform mat4			ViewMat;
	uniform float			SigmaScreen;
	uniform float			SigmaDepth;
	uniform int				nTaps;
	out vec4 				FragColor;

	//--------------------------------------------------------------------------
	float GaussianWeight(float _s, float _sigma)
	{
		float twoSigma2 = 2.f * _sigma * _sigma;
		float factor = 1.f / sqrt(3.141592654f * twoSigma2);
		return factor * exp(-(_s * _s) / twoSigma2);
	}
	//--------------------------------------------------------------------------
	void main()
	{
		vec2 rcpSize = 1.f / vec2(textureSize(InputTex,0).xy);
		vec2 pix 	 = gl_FragCoord.xy * rcpSize;
		vec4  dref	 = ViewMat * vec4(textureLod(PositionTex,pix,0).xyz,1);
		float cref	 = textureLod(InputTex, pix, 0).x;

		// We average the alpha channel since we use it for blending SSAO
		float color  = 0;
		float totalW = 0;
		for(int i=-nTaps;i<=nTaps;++i)
		{
			vec2  p	 = (gl_FragCoord.xy + i*Direction) * rcpSize;
			vec4  d	 = ViewMat * vec4(textureLod(PositionTex,p,0).xyz,1);
			float c	 = textureLod(InputTex,p,0).x;
			float w  = GaussianWeight(float(i),SigmaScreen) * GaussianWeight((d.z-dref.z),SigmaDepth);
			color 	+= w  * c;
			totalW  += w;
		}
		color	 /= totalW;
		FragColor = vec4(color);
	}
#endif
