#version 420 core

#ifdef CSM_RENDERER
	uniform	int						nCascades;
	#ifdef SSM
	uniform sampler2DArrayShadow	ShadowTex;
	#endif
	#if (defined VSM || defined EVSM)
	uniform sampler2DArray			ShadowTex;
	#endif
	uniform sampler2D				PositionTex;
	uniform sampler2D				DiffuseTex;
	uniform sampler2D				NormalTex;

	uniform vec3					ViewPos;
	uniform vec3					LightDir;
	uniform vec3					LightIntensity;
	uniform mat4					LightViewProjs[4];

	uniform float					BlendFactor;	// Fake variable
	uniform float 					Bias;

	out vec4 						FragColor;

	//--------------------------------------------------------------------------
	// Constants
	//--------------------------------------------------------------------------
	#define M_PI					3.141592654f
	#define INV_PI					0.3183098861f
	#define DISPLAY_CASCADES		0

	//--------------------------------------------------------------------------
	// Shadow test techniques
	//--------------------------------------------------------------------------
	#ifdef SSM
	float ShadowTest(const vec3 _pos, int _cascadeIndex, float _bias)
	{
		// Basic shadow test
		return texture(ShadowTex,vec4(_pos.xy,_cascadeIndex,_pos.z-_bias));
	}
	#endif
	//--------------------------------------------------------------------------
	#ifdef VSM
	float ShadowTest(const vec3 _pos, int _cascadeIndex, float _bias)
	{
		float tailCutoff = 0.15f*BlendFactor;
		vec2 moments = texture(ShadowTex,vec3(_pos.xy,_cascadeIndex)).xy;

		// Exit because result is undefined when occluder is further than the lit objet
		if(moments.x >= _pos.z - _bias)
			return 1.f;

		// Chebyshev inequality
		float variance	= moments.y - moments.x*moments.x;
		float delta		= _pos.z - moments.x;
		float pMax		= variance / (variance + delta*delta) - tailCutoff;
		return clamp(pMax,0.f,1.f);
	}
	#endif
	//--------------------------------------------------------------------------
	#ifdef EVSM
	float ShadowTest(const vec3 _pos, int _cascadeIndex, float _bias)
	{
		float k			 = K_EVSM_VALUE;
		float tailCutoff = 0.15f*BlendFactor;

		vec2 moments = texture(ShadowTex,vec3(_pos.xy,_cascadeIndex)).xy;

		// Exit because result is undefined when occluder is further than the lit objet
		if(moments.x >= exp(k * (_pos.z - _bias)))
			return 1.f;

		// Chebyshev inequality
		float variance	= moments.y - moments.x*moments.x;
		float delta		= exp(k * _pos.z) - moments.x;
		float pMax		= variance / (variance + delta*delta) - tailCutoff;
		return clamp(pMax,0.f,1.f);
	}
	#endif
	//--------------------------------------------------------------------------
	void main()
	{
		// Get world position of the point to shade
		vec2 pix			= gl_FragCoord.xy / vec2(textureSize(PositionTex,0));
		vec4 pos			= textureLod(PositionTex,pix,0);
		vec4 normal			= textureLod(NormalTex,pix,0);
		vec4 diffuse		= textureLod(DiffuseTex,pix,0);
		float roughness		= normal.w;
		float specularity	= diffuse.w;
		vec3 viewDir		= normalize(ViewPos-pos.xyz);

		// Select cascade
		// Compute derivates of position in projective light space for small 
		// variations in screen space
		vec3 lposs[4];
		for(int i=0;i<nCascades;++i)
		{
			vec4 current	= LightViewProjs[i] * vec4(pos.xyz,1);
			current.xyz	   += vec3(1);
			current.xyz	   *= 0.5f;
			lposs[i]		= current.xyz;
		}

		// Select cascade
		int cindex = 0;
		for(;cindex<nCascades;++cindex)
		{
			vec2 test1		= vec2(greaterThanEqual(lposs[cindex].xy,vec2(0,0)));
			vec2 test2		= vec2(lessThanEqual(lposs[cindex].xy,vec2(1,1)));

			if(int(dot(test1,test1)+dot(test2,test2))==4)
				break;
		}

		// Compute radiance
		float v		= ShadowTest(lposs[cindex].xyz, cindex, Bias);
//		float f		= WangBRDF(viewDir,-LightDir,normal.xyz,roughness,specularity),
		float f		= CookBRDF(viewDir,-LightDir,normal.xyz,roughness,specularity);

		#if DISPLAY_CASCADES
		vec3 color;
		{
				 if(cindex==0)
					color = vec3(1.f,0.f,0.f); 
			else if(cindex==1)
					color = vec3(0.f,1.f,0.f); 
			else if(cindex==2)
					color = vec3(0.f,0.f,1.f); 
			else if(cindex==3)
					color = vec3(1.f,1.f,0.f); 
			else 
					color = vec3(1.f,0.f,1.f);
		}
		FragColor   = vec4(f*LightIntensity*v*color,1.f);
		#else
		FragColor   = vec4(f*LightIntensity*v*diffuse.xyz,1.f);
		#endif

		#if LIGHTING_ONLY
		if(gl_FragCoord.x<10000)
			FragColor= vec4(vec3(f*LightIntensity*v),1.f);
		#endif
	}
#endif

#ifdef CSM_FILTER
	uniform sampler2DArray MomentTex;
	uniform vec2 Direction;
	in flat int gCascadeLayer;
	out vec4 FragColor;
	void main()
	{
		vec2 rcpSize	= 1.f/textureSize(MomentTex,0).xy;
		vec2 coord		= gl_FragCoord.xy;
		vec2 filtered	= vec2(0);
		for(int i=-2;i<=2;++i)
			filtered += textureLod(MomentTex,vec3((coord+i*Direction)*rcpSize,gCascadeLayer),0).xy;
		FragColor = vec4(filtered*0.2f,0,1);
	}
#endif

