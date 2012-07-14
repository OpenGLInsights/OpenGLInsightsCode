//------------------------------------------------------------------------------
#version 410

uniform vec2		PixelScale;
uniform sampler2D	BokehPositionTex; //(x,y,scale)
uniform sampler2D	BokehColorTex;
uniform float		MaxBokehRadius;
layout(location = ATTR_POSITION) in vec3 Position;
out float 			vRadius;
out float 			vDepth;
out vec4 			vColor;

void main()
{
	ivec2 bufSize, coord;
	bufSize 	 = textureSize(BokehPositionTex,0).xy;
	coord.y 	 = int(floor(gl_InstanceID/bufSize.y));
	coord.x 	 = gl_InstanceID - coord.y*bufSize.y;

	vColor		 = texelFetch(BokehColorTex,coord,0);
	vec4 pos	 = texelFetch(BokehPositionTex,coord,0);
	vRadius		 = pos.w * MaxBokehRadius;
	vDepth		 = pos.z;
	gl_Position	 = vec4((Position.xy+pos.xy)*PixelScale,0,1);
}

