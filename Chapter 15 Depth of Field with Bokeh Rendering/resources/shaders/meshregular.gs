#version 420 core

#ifdef CSM_BUILDER
	uniform int   nCascades;
	uniform mat4  Projections[MAX_CASCADES];
	uniform float Nears[MAX_CASCADES];
	uniform float Fars[MAX_CASCADES];

	layout(triangles) in;
	layout(triangle_strip, max_vertices = 12) out;
	out vec4 gLinearDepth;

	void main()
	{
		for(int layer=0;layer<nCascades;++layer)
		{
			gl_Layer = layer;
			for(int i=0; i<3;++i)
			{
				#ifdef SSM
				gl_Position = Projections[layer] * gl_in[i].gl_Position;
				#endif

				#if (defined VSM || defined EVSM)
				vec4 p		= Projections[layer] * gl_in[i].gl_Position;
				gLinearDepth= p;
				gl_Position = p;
				#endif

				EmitVertex();
			}
			EndPrimitive();
		}
	}
#endif
