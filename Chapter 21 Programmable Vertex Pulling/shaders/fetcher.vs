#version 420 core

layout(std140, binding = 0) uniform transform {
	mat4 ModelViewMatrix;
	mat4 ProjectionMatrix;
	mat4 MVPMatrix;
} Transform;

layout(binding = 1) uniform samplerBuffer attribBuffer;

out vec3 outVertexNormal;

out gl_PerVertex {
	vec4 gl_Position;
};

void main(void) {

	/* fetch attributes from texture buffer */
	vec3 inVertexPosition = texelFetch(attribBuffer, gl_VertexID * 2).xyz; 
	vec3 inVertexNormal = texelFetch(attribBuffer, gl_VertexID * 2 + 1).xyz;
	
	/* transform vertex and normal */
	outVertexNormal = mat3(Transform.ModelViewMatrix) * inVertexNormal;
	gl_Position = Transform.MVPMatrix * vec4(inVertexPosition, 1);
	
}
