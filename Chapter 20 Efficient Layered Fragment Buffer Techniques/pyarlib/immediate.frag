/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

#version 150

in vec4 colour;

out vec4 fragColour;
in vec2 coord;

uniform sampler2D tex;
uniform int enableTex;

void main()
{
	if (enableTex == 1)
		fragColour = texture(tex, coord);
	else
		fragColour = colour;
}
