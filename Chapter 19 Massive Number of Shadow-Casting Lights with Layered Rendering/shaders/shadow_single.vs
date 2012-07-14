#version 420 core

layout(location = 0) in vec3 inVertexPosition;

layout(std140, binding = 2) uniform lightTransform {
	mat4 VPMatrix[600];
} LightTransform;

layout(std140, binding = 1) uniform lightArray {
	struct lightType {
		vec4 position;
		vec4 color;
	} light[100];
} LightArray;

uniform int lightID;

out gl_PerVertex {
	vec4 gl_Position;
};

void main(void) {

	/* calculate clip space position */
	gl_Position = LightTransform.VPMatrix[lightID] * vec4(inVertexPosition, 1.f);

}
