#version 330
precision highp float;
precision highp int;

void main()
{
    gl_Position = vec4(vec2(gl_VertexID&1, (gl_VertexID&2)>>1)*2-1, 0, 1);
}
