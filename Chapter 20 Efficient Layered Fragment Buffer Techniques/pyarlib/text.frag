/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

#version 420

in vec2 coord;
in vec2 pix;

out vec4 fragColour;

uniform sampler2D font;
uniform vec3 colour;

void main()
{
	float a = texture(font, coord).r;
	fragColour = vec4(colour, a);
	
	/*
	float d = 0.05;
	if (pix.x < d || pix.x > 1.0-d ||
		pix.y < d || pix.y > 1.0-d)
		fragColour = vec4(1,0,0,1);
	*/
}
