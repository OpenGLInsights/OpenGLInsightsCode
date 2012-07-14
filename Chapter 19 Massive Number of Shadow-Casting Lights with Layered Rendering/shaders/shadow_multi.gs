#version 420 core

layout(std140, binding = 2) uniform lightTransform {
	mat4 VPMatrix[600];
} LightTransform;

layout(std140, binding = 1) uniform lightArray {
	struct lightType {
		vec4 position;
		vec4 color;
	} light[100];
} LightArray;

layout(triangles, invocations = 10) in;
layout(triangle_strip, max_vertices = 3) out;

layout(location = 0) in vec3 outVertexPosition[];

out gl_PerVertex {
	vec4 gl_Position;
};

void main() {

	vec3 normal = cross(outVertexPosition[2]-outVertexPosition[0], outVertexPosition[0]-outVertexPosition[1]);
	vec3 light = vec3(LightArray.light[gl_InvocationID].position) - outVertexPosition[0];
	
	if (dot(normal, light) > 0.f) {

		for (int i=0; i<3; ++i) {
			gl_Position = LightTransform.VPMatrix[gl_InvocationID] * vec4(outVertexPosition[i], 1.f);
			gl_Layer = gl_InvocationID;
			EmitVertex();
		}
		EndPrimitive();

	}
}
