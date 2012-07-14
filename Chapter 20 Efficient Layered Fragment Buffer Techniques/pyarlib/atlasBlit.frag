/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

#version 130

in vec2 texCoord;

uniform sampler2D tex;

out vec4 fragColour;

void main()
{
	fragColour = texture(tex, texCoord);
}

