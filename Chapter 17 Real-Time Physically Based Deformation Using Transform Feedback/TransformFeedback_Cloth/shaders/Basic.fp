/*
Demo code accompanying the Chapter: "Realtime Deformation using Transform Feedback" by
Muhammad Mobeen Movania and Lin Feng. This is the fragment shader for rendering rounded 
points in OpenGL 3.3.

Author: Muhammad Mobeen Movania
Last Modified: 9 September 2011.
*/
#version 330

smooth out vec4 vFragColor;
smooth in vec4 oColor;

void main()
{ 		
	//The equation is sqrt(dot(gl_PointCoord-0.5,gl_PointCoord-0.5)>0.5)	
	//squaring both sides gives this,
	if(dot(gl_PointCoord-0.5,gl_PointCoord-0.5)>0.25)	
		discard;
	else
		vFragColor = oColor;   
}