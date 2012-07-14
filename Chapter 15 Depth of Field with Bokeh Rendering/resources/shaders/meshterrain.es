#version 420 core

#ifdef GBUFFER
	uniform mat4		Transform;
	uniform ivec2		TileCount;
	uniform float		HeightFactor;
	uniform sampler2D	HeightTex;

	layout(quads, equal_spacing, ccw) in;

	patch in  	ivec2	cTileCoord;
	out 		vec2 	eTexCoord;
	out 		vec3 	ePosition;

	//------------------------------------------------------------------------------
	vec4 interpolate(in vec4 v0, in vec4 v1, in vec4 v2, in vec4 v3)
	{
		vec4 a = mix(v0, v1, gl_TessCoord.x);
		vec4 b = mix(v3, v2, gl_TessCoord.x);
		return mix(a, b, gl_TessCoord.y);
	}
	//------------------------------------------------------------------------------
	void main()
	{	
		vec2 coord	= (gl_TessCoord.xy + vec2(cTileCoord)) / vec2(TileCount);
		vec4 pos	= interpolate(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_in[2].gl_Position, gl_in[3].gl_Position);
		pos.z		+= HeightFactor * textureLod(HeightTex,coord,0).x;
		ePosition	= vec3(pos.xy,pos.z);
		gl_Position	= Transform * vec4(pos.xy,pos.zw);
		eTexCoord	= coord;
	}
#endif

#ifdef CSM_BUILDER
	uniform mat4		View;
	uniform ivec2		TileCount;
	uniform float		HeightFactor;
	uniform sampler2D	HeightTex;

	layout(quads, equal_spacing, ccw) in;

	patch in  	ivec2	cTileCoord;
	out 		vec2 	eTexCoord;

	//------------------------------------------------------------------------------
	vec4 interpolate(in vec4 v0, in vec4 v1, in vec4 v2, in vec4 v3)
	{
		vec4 a = mix(v0, v1, gl_TessCoord.x);
		vec4 b = mix(v3, v2, gl_TessCoord.x);
		return mix(a, b, gl_TessCoord.y);
	}
	//------------------------------------------------------------------------------
	void main()
	{	
		vec2 coord	= (gl_TessCoord.xy + vec2(cTileCoord)) / vec2(TileCount);
		vec4 pos	= interpolate(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_in[2].gl_Position, gl_in[3].gl_Position);
		pos.z		+= HeightFactor * textureLod(HeightTex,coord,0).x;
		gl_Position	= View * vec4(pos.xy,pos.zw);
		eTexCoord	= coord;
	}
#endif
