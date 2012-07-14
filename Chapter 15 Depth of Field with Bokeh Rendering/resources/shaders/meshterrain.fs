#version 420 core

#ifdef GBUFFER
	uniform sampler2D   DiffuseTex;
	uniform sampler2D   NormalTex;
	uniform float       Roughness;
	uniform float       Specularity;
	uniform float       TileFactor;

	in  vec3  ePosition;
	in  vec2  eTexCoord;

	layout(location = OUT_POSITION, 		index = 0) out vec4 FragPosition;
	layout(location = OUT_NORMAL_ROUGHNESS, index = 0) out vec4 FragNormal;
	layout(location = OUT_DIFFUSE_SPECULAR, index = 0) out vec4 FragDiffuse;

	void main()
	{
		FragPosition 	= vec4(ePosition,1);
		vec3 normal  	= textureLod(NormalTex,eTexCoord,0).xyz*2.f - 1.f;
		FragNormal		= vec4(normalize(normal),Roughness);
		FragDiffuse		= vec4(texture(DiffuseTex,eTexCoord*TileFactor).xyz,Specularity);
	}
#endif

#ifdef CSM_BUILDER
	#ifdef SSM
	void main()
	{

	}
	#endif

	#ifdef VSM
	in  vec4 gLinearDepth;
	out vec2 FragDepth;
	void main()
	{
		vec3 linearDepth = (gLinearDepth.xyz+vec3(1)) * 0.5f;
		vec2 outDepth;
		outDepth.x = linearDepth.z;
		outDepth.y = outDepth.x * outDepth.x;
		FragDepth  = outDepth;
	}
	#endif

	#ifdef EVSM
	in  vec4 gLinearDepth;
	out vec2 FragDepth;
	void main()
	{
		float k = K_EVSM_VALUE;
		vec3 linearDepth = (gLinearDepth.xyz+vec3(1)) * 0.5f;
		vec2 outDepth;
		outDepth.x = exp(k * linearDepth.z);
		outDepth.y = outDepth.x * outDepth.x;
		FragDepth  = outDepth;
	}
	#endif
#endif
