#version 420 core

uniform mat4 Transform;
uniform mat4 Model;

layout(location = ATTR_POSITION) in vec3 Position;
layout(location = ATTR_COLOR)    in vec3 Color;

out vec3 gColor;
void main()
{
	gl_Position = Transform * Model * vec4(Position,1.f);
	gColor		= Color;
}

