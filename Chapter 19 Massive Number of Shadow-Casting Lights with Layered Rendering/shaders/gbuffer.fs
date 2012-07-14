#version 420 core

layout(location = 0) in vec3 inInterpPosition;
layout(location = 1) in vec3 inInterpNormal;

layout(location = 0) out vec4 outPositionBuffer;
layout(location = 1) out vec3 outNormalBuffer;

void main(void) {

	/* output position */
	outPositionBuffer = vec4(inInterpPosition, 1.f);
	
	/* output renormalized normal */
	outNormalBuffer = normalize(inInterpNormal);;

}
