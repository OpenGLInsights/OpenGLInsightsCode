#version 420 core

//-----------------------------------------------------------------------------
//uniform mat4	 	Transformation;
uniform vec2		PixelScale;
in  float 			vRadius[1];
in  float 			vDepth[1];
in  vec4 			vColor[1];
out vec4			gColor;
out float			gDepth;
out vec2  			gTexCoord;

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;
//-----------------------------------------------------------------------------
void main()
{
	gl_Layer 	 = 0;
	gColor		 = vColor[0];
	gDepth		 = vDepth[0];
	vec2 offsetx = vec2(PixelScale.x*vRadius[0],0);
	vec2 offsety = vec2(0,PixelScale.y*vRadius[0]);
	vec2 offsets = vec2(-1,-1); // Screen offset

	// Expand point into a quad
	gl_Position = vec4(offsets + 2*(gl_in[0].gl_Position.xy - offsetx - offsety),0,1);
	gTexCoord	= vec2(0,0);
	EmitVertex();
	gl_Position = vec4(offsets + 2*(gl_in[0].gl_Position.xy + offsetx - offsety),0,1);
	gTexCoord	= vec2(1,0);
	EmitVertex();
	gl_Position = vec4(offsets + 2*(gl_in[0].gl_Position.xy - offsetx + offsety),0,1);
	gTexCoord	= vec2(0,1);
	EmitVertex();
	gl_Position = vec4(offsets + 2*(gl_in[0].gl_Position.xy + offsetx + offsety),0,1);
	gTexCoord	= vec2(1,1);
	EmitVertex();

	EndPrimitive();
}
