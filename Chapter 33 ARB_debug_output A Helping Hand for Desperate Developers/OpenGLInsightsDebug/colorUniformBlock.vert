#version 410


layout (std140) uniform Matrices {

	mat4 projMatrix;
	mat4 viewMatrix;
};

in vec4 position;
in vec4 color;

out vec4 ColorV;

void main()
{
	ColorV = color;

	gl_Position = projMatrix * viewMatrix * position ;
} 
