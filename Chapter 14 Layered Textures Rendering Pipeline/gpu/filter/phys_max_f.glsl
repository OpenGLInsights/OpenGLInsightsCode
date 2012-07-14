#version 150 core

uniform usampler2D unit_sten;
uniform sampler2D unit_color;

noperspective in vec2 tex_coord;
out uint to_sten;
out vec2 to_color;

const uint mask = 0xFFu;
const ivec2 off = ivec2(-1.0,0.0);


void main()	{
	to_sten = 0u; to_color = vec2(0.0); return;

	//8b stencil + 24b depth
	uint t0 = texture( unit_sten, tex_coord ).x;
	uint t1 = textureOffset( unit_sten, tex_coord, off.xy ).x;
	uint t2 = textureOffset( unit_sten, tex_coord, off.yx ).x;
	uint st = max( t0&mask, max(t1&mask,t2&mask) );

	//color (object ID)
	vec2 c0 = texture( unit_color, tex_coord ).xy;
	to_color = c0;
	to_sten = t0;
	if(st == 0u) return;

	uint k0 = uint(step(st,t0));
	uint k1 = uint(step(st,t1));
	uint k2 = uint(step(st,t2));
	uint depth = k0*(t0>>8u) + k1*(t1>>8u) + k2*(t2>>8u);
	to_sten = st + (depth<<8u);

	vec2 c1 = textureOffset( unit_color, tex_coord, off.xy ).xy;
	vec2 c2 = textureOffset( unit_color, tex_coord, off.yx ).xy;
	to_color = k0*c0 + k1*c1 + k2*c2;
}