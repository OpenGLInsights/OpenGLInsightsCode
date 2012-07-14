#version 330

layout (std140) uniform Matrices {

	mat4 projMatrix;
	mat4 modelviewMatrix;
	mat4 projModelViewMatrix;
	mat3 normalMatrix;
};

in vec3 position;
in vec3 normal;
in vec2 texCoord;

out vec2 TexCoord;
out vec3 Normal;

void main()
{
//	Normal = normalize(vec3(modelviewMatrix * vec4(normal, 0.0)));
	Normal = normalize(normalMatrix * normal);	
	TexCoord = vec2(texCoord);
	gl_Position = projModelViewMatrix * vec4(position, 1.0);
}