#version 150 core

uniform float k_dark;

void main()	{
	gl_FragDepth = exp2( k_dark*(gl_FragCoord.z-1.0) );
}
