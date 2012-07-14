#version 420 core

#ifdef CSM_FILTER
	layout(triangles) in;
	layout(triangle_strip, max_vertices = 3) out;
	in int vCascadeLayer[];
	out flat int gCascadeLayer;

	void main()
	{
		gCascadeLayer	= vCascadeLayer[0];
		gl_Layer		= vCascadeLayer[0];
		gl_Position		= gl_in[0].gl_Position; EmitVertex();
		gl_Position		= gl_in[1].gl_Position; EmitVertex();
		gl_Position		= gl_in[2].gl_Position; EmitVertex();
		EndPrimitive();
	}
#endif
