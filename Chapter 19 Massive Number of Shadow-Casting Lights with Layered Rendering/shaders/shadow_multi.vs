#version 420 core

layout(location = 0) in vec3 inVertexPosition;

layout(location = 0) out vec3 outVertexPosition;

void main(void) {

	/* pass through vertex position */
	outVertexPosition = inVertexPosition;

}
