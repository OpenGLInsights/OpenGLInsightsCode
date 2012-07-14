#version 120

varying vec2 vTexCoord2D;

void main(void) {
	vTexCoord2D = gl_Vertex.xy * 8.0;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
