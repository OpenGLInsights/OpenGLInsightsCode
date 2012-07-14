/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

#version 420

in vec4 osVert;

uniform mat4 modelviewMat;
uniform mat4 projectionMat;

void main()
{
	gl_Position = projectionMat * modelviewMat * osVert;
}
