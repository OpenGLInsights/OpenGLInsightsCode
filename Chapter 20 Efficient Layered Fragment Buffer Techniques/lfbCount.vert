/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

#version 420

in vec4 osVert;

uniform mat4 projectionMat;
uniform mat4 modelviewMat;

void main()
{
	gl_Position = projectionMat * modelviewMat * osVert;
}
