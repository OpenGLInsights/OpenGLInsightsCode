#version 330 core

in vec3 outVertexNormal;

layout(location = 0) out vec4 outColor;

void main(void) {

	/* output normal */
	outColor = vec4(outVertexNormal, 1.0);

}
