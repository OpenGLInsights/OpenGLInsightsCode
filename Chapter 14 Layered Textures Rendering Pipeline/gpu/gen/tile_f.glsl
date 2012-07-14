#version 150 core
//Tile-Based Texture Mapping on Graphics Hardware
//Li-Yi Wei
//SIGGRAPH/EUROGRAPHICS Conference on Graphics Hardware, 2004.

uniform sampler1D unit_perm;
uniform sampler2D unit_tile;
uniform vec2 map_scale;		//Mh,Mv
uniform vec2 tile_scale;	//Ph,Pv
uniform vec2 num_colors;	//Kh,Kv


float edge_order(vec2 q)	{
	if(q.x < q.y) return 2*q.x + q.y*q.y;
	if(q.x == q.y)	{
		if(q.x>0.0) return q.x*q.x + 2.0*q.x - 1.0;
		return 0.0;
	}else	{
		if(q.y>0.0) return q.x*q.x + 2.0*q.y - 1.0;
		return q.x*q.x + 2.0*q.x;
	}
	return 0.0;
}


vec4 tile_sample(vec2 tc)	{
	vec2 thisTile = floor( fract(tc) * map_scale );
	vec2 nextTile = mod( thisTile + vec2(1.0), map_scale );
	vec2 help = vec2(
		texture(unit_perm, 1.0*thisTile.x ).x,
		texture(unit_perm, 2.0*thisTile.y ).x
		);
	vec4 edges = vec4(
		texture(unit_perm, help.x + thisTile.y ).x,
		texture(unit_perm, help.y + nextTile.x ).x,
		texture(unit_perm, help.x + nextTile.y ).x,
		texture(unit_perm, help.y + thisTile.x ).x
		);
	edges = mod( edges, num_colors.xyxy );
	vec2 inTile = vec2(
		edge_order( edges.wy ),
		edge_order( edges.xz )
		);
	vec2 mapAd = tc * map_scale;
	vec2 tileTex = mapAd / tile_scale;
	vec2 coord = (inTile + fract(mapAd)) / tile_scale;
	return texture(unit_tile, coord);
}