/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

#version 420

in vec4 osVert;
in vec3 osNorm;
in vec2 texCoord;

uniform mat4 projectionMat;
uniform mat4 modelviewMat;
uniform mat3 normalMat;
uniform vec4 lightPos;

out vec3 osFrag;
out vec3 esFrag;
out vec3 esLight;
out vec3 esNorm;
out vec2 coord;

void main()
{
	coord = texCoord;
	osFrag = osVert.xyz;
	vec4 esVert = modelviewMat * osVert;
	vec4 csVert = projectionMat * esVert;
	gl_Position = csVert;

	esNorm = normalize(normalMat * osNorm);
	esFrag = normalize(esVert.xyz);
	esLight = normalize(lightPos.xyz - esVert.xyz);
}
