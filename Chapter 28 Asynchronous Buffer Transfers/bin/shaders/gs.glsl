#version 330
precision highp float;
precision highp int;

//comment out here and in vs.glsl to get the second problem
//#define BUG_2

////////////////////////////////////////////////////////////////////////////////
in _struct_TreeBilboNode {
  vec4 pos;
  vec4 norm_h;
  vec3 bilvec;
#ifdef BUG_2
  vec4 attenuation;
  vec4 light;
#endif
} p[1];

out _struct_TreeBilboVertex {
  vec2 coord;
  flat out int jj;
} v;

uniform mat4 modelViewProj_6;
uniform float invfarplanecoef_6;
layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

void main()
{
    if(p[0].pos.w != 0)
    {
        vec3 bilvec = 4.5 * (1 + 0.5*p[0].norm_h.w) * p[0].bilvec;
        vec3 height = 30 * (1 + 0.5*p[0].norm_h.w) * p[0].norm_h.xyz;

        vec3 tpos;
        tpos = p[0].pos.xyz + bilvec;
        gl_Position = modelViewProj_6 * vec4(tpos, 1);
        v.coord = vec2(1,1);
        v.jj=1;
        EmitVertex();

        tpos = p[0].pos.xyz + bilvec + height;
        gl_Position = modelViewProj_6 * vec4(tpos, 1);
        v.coord = vec2(1,0);
        v.jj=1;
        EmitVertex();

        tpos = p[0].pos.xyz - bilvec;
        gl_Position = modelViewProj_6 * vec4(tpos, 1);
        v.coord = vec2(0,1);
        v.jj=1;
        EmitVertex();

        tpos = p[0].pos.xyz - bilvec + height;
        gl_Position = modelViewProj_6 * vec4(tpos, 1);
        v.coord = vec2(0,0);
        v.jj=1;
        EmitVertex();
    }
}
