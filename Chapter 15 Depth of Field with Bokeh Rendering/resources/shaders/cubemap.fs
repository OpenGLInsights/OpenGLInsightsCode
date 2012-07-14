#version 420 core

uniform samplerCube 	EnvTex;
in  vec3 				gPosition;
out vec4 				FragColor;

void main()
{
	FragColor = textureLod(EnvTex,gPosition.xyz,0);
}
