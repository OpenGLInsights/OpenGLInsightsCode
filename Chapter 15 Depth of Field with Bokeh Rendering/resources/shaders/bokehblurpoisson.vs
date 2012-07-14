//-----------------------------------------------------------------------------
#version 410

layout(location = ATTR_POSITION) in vec2 Position;

void main()
{
	gl_Position  = vec4(Position,0,1);
}

