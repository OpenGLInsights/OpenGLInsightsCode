/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

#version 420

in vec4 osVert;
in vec3 osNorm;
in vec2 texCoord;

uniform mat4 projectionMat;
uniform mat4 modelviewMat;
uniform mat3 normalMat;

out vec3 esNorm;
out vec3 esFrag;
out vec3 osFrag;
out vec2 coord;

void main()
{
	coord = texCoord;
	osFrag = osVert.xyz;
	vec4 esVert = modelviewMat * osVert;
	esFrag = esVert.xyz;
	gl_Position = projectionMat * esVert;
	esNorm = normalMat * osNorm;
}
