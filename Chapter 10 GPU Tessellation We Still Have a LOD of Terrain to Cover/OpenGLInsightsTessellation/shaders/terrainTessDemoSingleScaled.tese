#version 410

layout(quads, fractional_even_spacing, cw) in;

layout (std140) uniform Matrices {

	mat4 projMatrix;
	mat4 modelviewMatrix;
	mat4 projModelViewMatrix;
	mat3 normalMatrix;
};

uniform sampler2D heightMap;
uniform float heightStep;
uniform float gridSpacing;
uniform int scaleFactor;
uniform mat4 pvm;
uniform float patchSize;


in vec2 posTC[];

out vec2 uvTE;



#define sizeFactor 1.0/patchSize
#define uv gl_TessCoord


float height(float u, float v) {

	return (texture(heightMap, vec2(u,v)).r * heightStep);
}


void main() {

	ivec2 tSize = textureSize(heightMap,0) * scaleFactor;
	float div = tSize.x * sizeFactor;

	// Compute texture coordinates
//	uvTE.s = (posTC[0].x + uv.s/div) ;
//	uvTE.t = (posTC[0].y + uv.t/div) ;
	uvTE = posTC[0].xy + uv.st/div;

	// compute vertex position [0 .. tSize * gridSpacing]
	vec4 res;
	res.x = uvTE.s * tSize.x * gridSpacing;
	res.z = uvTE.t * tSize.y * gridSpacing;
	res.y = height(uvTE.s, uvTE.t) ;
	res.w = 1.0;
	
	//gl_Position = pvm * res;
	gl_Position = projModelViewMatrix * res;
}

