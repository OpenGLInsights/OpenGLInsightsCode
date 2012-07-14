#version 120

varying vec2 vTexCoord2D;
varying vec3 vTexCoord3D;

void main(void) {
	vTexCoord2D = gl_MultiTexCoord0.xy * 16.0;
	vTexCoord3D = gl_Vertex.xyz * 8.0;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
