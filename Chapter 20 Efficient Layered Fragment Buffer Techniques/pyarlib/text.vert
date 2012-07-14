/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

#version 420

in vec3 intdata;

out int pglyph;
out ivec2 ppos;

void main()
{
	pglyph = int(intdata.x);
	ppos = ivec2(intdata.yz);
}
