#version 420 core

//-----------------------------------------------------------------------------
uniform mat4 Transformations[6];
//-----------------------------------------------------------------------------
out vec3 gPosition;
layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;
//-----------------------------------------------------------------------------
void main()
{
	for(int layer=0;layer<6;++layer)
	{
		gl_Layer = layer;
		for(int i=0; i<3;++i)
		{
			gl_Position = gl_in[i].gl_Position;
			gPosition	= (Transformations[layer] * gl_in[i].gl_Position).xyz;
			EmitVertex();
		}
		EndPrimitive();
	}
}
