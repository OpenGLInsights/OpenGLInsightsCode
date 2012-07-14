#version 420 core

#ifdef GBUFFER
	uniform			float ProjFactor;
	uniform 		float TessFactor;

	layout(vertices = 4) out;
	layout(vertices = 4) in;
	in  			ivec2 vTileCoord[];
	in				vec3  vProjPosition[];
	patch out  		ivec2 cTileCoord;


	void main()
	{
		// Version based on edge size
		barrier();

		// Evaluate in projective space what is the size of triangle's edges
		float edgeFactor[4];
		edgeFactor[0]	 					= clamp(length(vProjPosition[3].xyz - vProjPosition[0].xyz) * ProjFactor,0.0f,1.f);
		edgeFactor[1]	 					= clamp(length(vProjPosition[0].xyz - vProjPosition[1].xyz) * ProjFactor,0.0f,1.f);
		edgeFactor[2]					 	= clamp(length(vProjPosition[1].xyz - vProjPosition[2].xyz) * ProjFactor,0.0f,1.f);
		edgeFactor[3] 	 					= clamp(length(vProjPosition[2].xyz - vProjPosition[3].xyz) * ProjFactor,0.0f,1.f);

		// Deduce tesselation factor
		float innerScale 	 				= 0.25f * (edgeFactor[0] + edgeFactor[1] + edgeFactor[2] + edgeFactor[3]);
		gl_TessLevelInner[0] 				= innerScale * TessFactor;
		gl_TessLevelInner[1] 				= innerScale * TessFactor;
		gl_TessLevelOuter[gl_InvocationID] 	= edgeFactor[gl_InvocationID] * TessFactor;

		gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
		cTileCoord 							= vTileCoord[gl_InvocationID];
	}
#endif


#ifdef CSM_BUILDER
	uniform			float ProjFactor;
	uniform 		float TessFactor;

	layout(vertices = 4) out;
	layout(vertices = 4) in;
	in  			ivec2 vTileCoord[];
	in				vec3  vProjPosition[];
	patch out  		ivec2 cTileCoord;


	void main()
	{
		// Version based on edge size
		barrier();

		// Evaluate in projective space what is the size of triangle's edges
		float edgeFactor[4];
		edgeFactor[0]	 					= clamp(length(vProjPosition[3].xyz - vProjPosition[0].xyz) * ProjFactor,0.0f,1.f);
		edgeFactor[1]	 					= clamp(length(vProjPosition[0].xyz - vProjPosition[1].xyz) * ProjFactor,0.0f,1.f);
		edgeFactor[2]					 	= clamp(length(vProjPosition[1].xyz - vProjPosition[2].xyz) * ProjFactor,0.0f,1.f);
		edgeFactor[3] 	 					= clamp(length(vProjPosition[2].xyz - vProjPosition[3].xyz) * ProjFactor,0.0f,1.f);

		// Deduce tesselation factor
		float innerScale 	 				= 0.25f * (edgeFactor[0] + edgeFactor[1] + edgeFactor[2] + edgeFactor[3]);
		gl_TessLevelInner[0] 				= innerScale * TessFactor;
		gl_TessLevelInner[1] 				= innerScale * TessFactor;
		gl_TessLevelOuter[gl_InvocationID] 	= edgeFactor[gl_InvocationID] * TessFactor;

		gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
		cTileCoord 							= vTileCoord[gl_InvocationID];
	}
#endif
