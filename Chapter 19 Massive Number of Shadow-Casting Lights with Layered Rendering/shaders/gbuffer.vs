#version 420 core

layout(std140, binding = 0) uniform transform {
	mat4 ModelViewMatrix;
	mat4 ProjectionMatrix;
	mat4 MVPMatrix;
	vec4 Viewport;
	float zNear;
	float zFar;
	float nearTop;
	float nearRight;
} Transform;

layout(location = 0) in vec3 inVertexPosition;
layout(location = 1) in vec3 inVertexNormal;

layout(location = 0) out vec3 outVertexPosition;
layout(location = 1) out vec3 outVertexNormal;

out gl_PerVertex {
	vec4 gl_Position;
};

void main(void) {

	/* pass through world space position */
	outVertexPosition = inVertexPosition;
	
	/* pass through world space normal */
	outVertexNormal = inVertexNormal;
	
	/* transform position into clip space */
	gl_Position = Transform.MVPMatrix * vec4(inVertexPosition, 1);
	
}
