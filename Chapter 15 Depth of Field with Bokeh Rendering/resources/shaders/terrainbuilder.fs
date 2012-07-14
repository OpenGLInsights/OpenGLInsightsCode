#version 420

#ifdef NORMAL_BUILDER
	uniform sampler2D   HeightTex;
	uniform float       HeightFactor;
	uniform vec2        TerrainSize;
	out vec4            FragColor;

	//--------------------------------------------------------------------------
	float XSobel3x3(ivec2 _coord)
	{
		float gradient = 0;
		gradient   += texelFetch(HeightTex, _coord + ivec2(-1,-1), 0).x * HeightFactor * -1;
		gradient   += texelFetch(HeightTex, _coord + ivec2(-1, 0), 0).x * HeightFactor * -3;
		gradient   += texelFetch(HeightTex, _coord + ivec2(-1, 1), 0).x * HeightFactor * -1;

		gradient   += texelFetch(HeightTex, _coord + ivec2( 1,-1), 0).x * HeightFactor *  1;
		gradient   += texelFetch(HeightTex, _coord + ivec2( 1, 0), 0).x * HeightFactor *  3;
		gradient   += texelFetch(HeightTex, _coord + ivec2( 1, 1), 0).x * HeightFactor *  1;

		return gradient * 0.2f; // Divide by the sum of weights
	}
	//--------------------------------------------------------------------------
	float YSobel3x3(ivec2 _coord)
	{
		float gradient = 0;
		gradient   += texelFetch(HeightTex, _coord + ivec2(-1,-1), 0).x * HeightFactor * -1;
		gradient   += texelFetch(HeightTex, _coord + ivec2( 0,-1), 0).x * HeightFactor * -3;
		gradient   += texelFetch(HeightTex, _coord + ivec2( 1,-1), 0).x * HeightFactor * -1;

		gradient   += texelFetch(HeightTex, _coord + ivec2(-1, 1), 0).x * HeightFactor *  1;
		gradient   += texelFetch(HeightTex, _coord + ivec2( 0, 1), 0).x * HeightFactor *  3;
		gradient   += texelFetch(HeightTex, _coord + ivec2( 1, 1), 0).x * HeightFactor *  1;

		return gradient * 0.2f; // Divide by the sum of weights
	}
	//--------------------------------------------------------------------------
	void main()
	{
		ivec2 coord  = ivec2(floor(gl_FragCoord.xy));
		vec2 rcpSize = TerrainSize / vec2(textureSize(HeightTex,0));
		vec3 nx		 = vec3(rcpSize.x,0,XSobel3x3(coord));
		vec3 ny		 = vec3(0,rcpSize.y,YSobel3x3(coord));
		vec3 n		 = normalize(cross(nx,ny));
		FragColor	 = vec4((n + vec3(1.f)) * 0.5f,1);
	}
#endif
