/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

#version 150

in vec4 osVert;
in vec4 vertColour;
in vec2 texCoord;

uniform mat4 projectionMat;
uniform mat4 modelviewMat;

out vec4 colour;
out vec2 coord;

void main()
{
	coord = texCoord;
	colour = vertColour/255.0;
	gl_Position = projectionMat * modelviewMat * osVert;
}
