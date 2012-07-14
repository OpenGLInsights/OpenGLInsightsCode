#version 420 core

uniform mat4 Transformation;
layout(location = ATTR_POSITION) in	vec3 Position;
out	vec3 gPosition;

void main()
{
	gl_Position = Transformation * vec4(Position,1.f);
	gPosition	= Position;
}

