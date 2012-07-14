/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

#version 140

out vec4 fragColour;

in vec2 coord;

uniform sampler2D tex;

uniform int test;

void main()
{
	fragColour = texture(tex, coord);
	if (test == 1)
		fragColour = vec4(1, 0, 0, 1);
}
