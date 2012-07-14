/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

#version 130

in vec4 osVert;
out vec2 texCoord;

uniform vec2 pos;
uniform vec2 size;

void main()
{
	texCoord = osVert.xy;
	vec2 pos = osVert.xy * size + pos;
	gl_Position = vec4(pos * 2.0 - 1.0, 0.0, 1.0);
}
