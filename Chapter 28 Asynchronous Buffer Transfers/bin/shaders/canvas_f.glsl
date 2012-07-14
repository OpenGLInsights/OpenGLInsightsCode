/*
Copyright (C) 2011 by Ladislav Hrabcak

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#version 140
//#version 330
precision highp float;
precision highp int;

uniform sampler2D tex_font;
uniform vec4 clip;

in vec2 uv;
in vec4 color;
flat in int color_only;

out vec4 _retval;

void main()
{
	vec4 col;

	if(color_only!=1)
		col=textureLod(tex_font,uv/vec2(textureSize(tex_font,0)),0);
	else
		col=vec4(1);

	if(col.a==0.0f 
		|| gl_FragCoord.x<clip.x
		|| gl_FragCoord.x>clip.z
		|| gl_FragCoord.y<clip.y
		|| gl_FragCoord.y>clip.w)
		discard;

	_retval = col * color;
}
