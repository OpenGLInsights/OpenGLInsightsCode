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

//#version 330
#version 140
precision highp float;
precision highp int;

uniform samplerBuffer tb_elements;
uniform vec2 screen_size;	//< 1.0 / screen_size
uniform int start_index;

out vec2 uv;
out vec4 color;
flat out int color_only;

void main()
{
	int index = start_index + gl_InstanceID * 3;

	vec4 pos = texelFetch(tb_elements, index) * vec4(screen_size, screen_size);
	vec4 texpos = texelFetch(tb_elements, index + 1);
	color = texelFetch(tb_elements, index + 2);

	vec2 local_pos = vec2(gl_VertexID & 1, (gl_VertexID & 2) >> 1);
	
	uv.xy = texpos.xy + local_pos * texpos.zw;
	pos.xy += local_pos * pos.zw;
    gl_Position = vec4(pos.xy * 2.0 - 1.0, 0, 1);

	color_only = texpos.z==0 ? 1 : 0;
}
