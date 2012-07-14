/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

#version 140

in vec4 osVert;
in vec2 texCoord;

uniform mat4 transformMat;

out vec2 coord;

void main()
{
	coord = texCoord;
	gl_Position = transformMat * osVert;
}
