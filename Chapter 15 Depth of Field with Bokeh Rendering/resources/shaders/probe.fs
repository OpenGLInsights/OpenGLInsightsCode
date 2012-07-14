#version 420 core

//------------------------------------------------------------------------------
// Constants
//------------------------------------------------------------------------------
#define INV_PI          0.3183098861f
#define M_PI            3.1415926535f

#ifdef BUILDER
	uniform samplerCube  CubeTex;
	uniform mat4         Transformations[6];

	layout(location = OUT_COEFF0, index = 0) out vec4 SHCoeffs0;
	layout(location = OUT_COEFF1, index = 0) out vec4 SHCoeffs1;
	layout(location = OUT_COEFF2, index = 0) out vec4 SHCoeffs2;
	layout(location = OUT_COEFF3, index = 0) out vec4 SHCoeffs3;
	layout(location = OUT_COEFF4, index = 0) out vec4 SHCoeffs4;
	layout(location = OUT_COEFF5, index = 0) out vec4 SHCoeffs5;
	layout(location = OUT_COEFF6, index = 0) out vec4 SHCoeffs6;

	void main()
	{
		// Compute solid angle weight
		// Each face has a 2x2 unit surface placed at 1 unit from the center
		// Thus the area of a face is 4
		// The solid angle is :
		// \Omega = cos\theta / r2 * texelArea
		// cos\theta is equal to 1/sqrt(r2)	
		ivec2 texSize	= textureSize(CubeTex,0).xy;
		float texArea	= 4.f/float(texSize.x*texSize.y); 
		vec2  texCoord	= (gl_FragCoord.xy) / vec2(texSize);
		vec2  texelPos	= (gl_FragCoord.xy + vec2(0.5f)) / vec2(texSize)*2.f - vec2(1.f);
		float r2		= texelPos.x*texelPos.x + texelPos.y*texelPos.y + 1;
		float weight	= 1.f / (sqrt(r2)*r2) * texArea;
		vec3 refDir		= normalize(vec3(texelPos,-1));

		vec3 SHCoeffs[9];
		SHCoeffs[0] = vec3(0);
		SHCoeffs[1] = vec3(0);
		SHCoeffs[2] = vec3(0);
		SHCoeffs[3] = vec3(0);
		SHCoeffs[4] = vec3(0);
		SHCoeffs[5] = vec3(0);
		SHCoeffs[6] = vec3(0);
		SHCoeffs[7] = vec3(0);
		SHCoeffs[8] = vec3(0);

		for(int l=0;l<6;++l)
		{
			vec3 dir	 = (Transformations[l] * vec4(refDir,0)).xyz;
			vec3 value   = weight * textureLod(CubeTex,dir,0).xyz;

			// Compute SH function
			SHCoeffs[0] += value * 0.282095;
			SHCoeffs[1] += value * 0.488603 *  dir.y;
			SHCoeffs[2] += value * 0.488603 *  dir.z;
			SHCoeffs[3] += value * 0.488603 *  dir.x;
			SHCoeffs[4] += value * 1.092548 *  dir.x*dir.y;
			SHCoeffs[5] += value * 1.092548 *  dir.y*dir.z;
			SHCoeffs[6] += value * 0.315392 * (3.f*dir.z*dir.z -1.f);
			SHCoeffs[7] += value * 1.092548 *  dir.x * dir.z;
			SHCoeffs[8] += value * 0.546274 * (dir.x*dir.x - dir.y*dir.y);
		}

		SHCoeffs0 = vec4(SHCoeffs[0],SHCoeffs[7].x);
		SHCoeffs1 = vec4(SHCoeffs[1],SHCoeffs[7].y);
		SHCoeffs2 = vec4(SHCoeffs[2],SHCoeffs[7].z);
		SHCoeffs3 = vec4(SHCoeffs[3],SHCoeffs[8].x);
		SHCoeffs4 = vec4(SHCoeffs[4],SHCoeffs[8].y);
		SHCoeffs5 = vec4(SHCoeffs[5],SHCoeffs[8].z);
		SHCoeffs6 = vec4(SHCoeffs[6],1);
	}
#endif


#ifdef RENDERER
	#if DIFFUSE_REFLECTION
	uniform sampler2D		NormalTex;
	uniform sampler2D		DiffuseTex;
	uniform vec3			SHCoeffs[9];

	out vec4 				FragColor;
	const float 			c1 = 0.429043, 
							c2 = 0.511664, 
							c3 = 0.743125, 
							c4 = 0.886227, 
							c5 = 0.247708;

	// Convolve with a clamped cos
	// From Siggraph 02 An efficient representation for irradiance environment maps 
	// [Ravi Ramamorthi, Pat Hanrahan]
	// Compute the value of the kernel (including the scaling factor of the convolution)
	void main()
	{
		vec2 pix		= gl_FragCoord.xy / vec2(textureSize(NormalTex,0));
		vec3 n			= normalize(texture(NormalTex,pix).xyz);
		vec4 color		= texture(DiffuseTex,pix);

		vec3 dRadiance	= 	c1 *  SHCoeffs[8] * (n.x*n.x - n.y*n.y) 
						+	c3 *  SHCoeffs[6] * n.z*n.z
						+	c4 *  SHCoeffs[0]
						-	c5 *  SHCoeffs[6] 
						+ 2*c1 * (SHCoeffs[4]*n.x*n.y + SHCoeffs[7]*n.x*n.z + SHCoeffs[5]*n.y*n.z)
						+ 2*c2 * (SHCoeffs[3]*n.x + SHCoeffs[1]*n.y + SHCoeffs[2]*n.z );

		FragColor		= vec4(color.xyz * dRadiance * INV_PI,1);

		#if LIGHTING_ONLY
		if(gl_FragCoord.x<10000)
			FragColor	= vec4(vec3(dRadiance * INV_PI),1);
		#endif
	}
	#endif
	#if TOTAL_REFLECTION
	uniform samplerCube		CubeTex;
	uniform sampler2D		NormalTex;
	uniform sampler2D		DiffuseTex;
	uniform sampler2D		PositionTex;
	uniform vec3			SHCoeffs[9];
	uniform vec3			ViewPos;

	out vec4 				FragColor;
	const float 			c1 = 0.429043, 
							c2 = 0.511664, 
							c3 = 0.743125, 
							c4 = 0.886227, 
							c5 = 0.247708;
	//--------------------------------------------------------------------------
	float MipmapLevel(float _exponent, int _cubeResolution)
	{
		// Magic fomula : derived from Wang09 for a cubemap
		// Epsilon is set up to 0.9
		// Assume each pixel has the same solid angle
		// -3*ln(0.9) = 0.316081547
		return clamp(0.5 * log2(0.316081547 *_cubeResolution*_cubeResolution / _exponent),0,log2(_cubeResolution));
	}
	//--------------------------------------------------------------------------
	// Convolve with a clamped cos
	// From Siggraph 02 An efficient representation for irradiance environment maps 
	// [Ravi Ramamorthi, Pat Hanrahan]
	// Compute the value of the kernel (including the scaling factor of the convolution)
	void main()
	{
		vec2 pix		= gl_FragCoord.xy / vec2(textureSize(NormalTex,0));
		vec3 p			= normalize(texture(PositionTex,pix).xyz);
		vec4 n_rough	= texture(NormalTex,pix);
		vec3 n			= normalize(n_rough.xyz);
		float roughness	= n_rough.w;
		vec3 vDirection	= normalize(ViewPos-p);

		vec4 color		= texture(DiffuseTex,pix);
		vec3 dRadiance	= 	c1 *  SHCoeffs[8] * (n.x*n.x - n.y*n.y) 
						+	c3 *  SHCoeffs[6] * n.z*n.z
						+	c4 *  SHCoeffs[0]
						-	c5 *  SHCoeffs[6] 
						+ 2*c1 * (SHCoeffs[4]*n.x*n.y + SHCoeffs[7]*n.x*n.z + SHCoeffs[5]*n.y*n.z)
						+ 2*c2 * (SHCoeffs[3]*n.x + SHCoeffs[1]*n.y + SHCoeffs[2]*n.z );

		// Compute BRDF lobe
		vec3  rDirection;
		float rExponent,rScale;
		WangWrap(vDirection,n,roughness,rDirection,rExponent,rScale);
		float rLevel	= MipmapLevel(rExponent,textureSize(CubeTex,0).x);
		vec3 sRadiance	= textureLod(CubeTex,rDirection,rLevel).xyz;
		FragColor		= vec4(color.xyz * (color.w*sRadiance + dRadiance * INV_PI),1);

		#if LIGHTING_ONLY
		if(gl_FragCoord.x<10000)
			FragColor	= vec4(vec3(color.w*sRadiance + dRadiance * INV_PI),1);
		#endif
	}
	#endif
#endif
