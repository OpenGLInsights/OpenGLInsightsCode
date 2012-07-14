/*
Demo code accompanying the Chapter: "Realtime Deformation using Transform Feedback" by
Muhammad Mobeen Movania and Lin Feng. This is the shader code for passthrough fragment processing. 

Author: Muhammad Mobeen Movania
Last Modified: 9 September 2011.
*/
#version 330

smooth out vec4 vFragColor;
uniform vec4 vColor;

void main()
{ 		
	vFragColor = vColor;  	
}