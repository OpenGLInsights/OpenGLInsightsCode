#version 420 core

#ifdef GBUFFER
	uniform sampler2D   DiffuseTex;
	uniform sampler2D   NormalTex;
	uniform float       Roughness;
	uniform float       Specularity;

	in  vec3  vPosition;
	in  vec3  vNormal;
	in  vec3  vTangent;
	in  vec2  vTexCoord;
	in  float vTBNsign;

	layout(location = OUT_POSITION, 		index = 0) out vec4 FragPosition;
	layout(location = OUT_NORMAL_ROUGHNESS, index = 0) out vec4 FragNormal;
	layout(location = OUT_DIFFUSE_SPECULAR, index = 0) out vec4 FragDiffuse;

	void main()
	{
		// Build tangent space
		vec3 vNNormal   = normalize(vNormal);
		vec3 vNTangent  = normalize(vTangent);
		vec3 vNBitangent= normalize(cross(vNNormal,vNTangent)) * sign(vTBNsign);

		// Extract normal and project it in world space
		vec3 normal  	= texture(NormalTex,vTexCoord).xyz*2.f - 1.f;
		FragPosition 	= vec4(vPosition,1);
		FragNormal   	= vec4(normalize(normal.x*vNTangent + normal.y*vNBitangent + normal.z*vNNormal),Roughness);
		FragDiffuse  	= vec4(texture(DiffuseTex,vTexCoord).xyz,Specularity);
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
		outDepth.x = exp(linearDepth.z * k);
		outDepth.y = outDepth.x * outDepth.x;
		FragDepth  = outDepth;
	}
	#endif
#endif
