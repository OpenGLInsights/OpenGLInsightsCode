#version 330

layout (std140) uniform Material {
	vec4 diffuse;
	vec4 ambient;
	vec4 specular;
	vec4 emissive;
	float shininess;
	int texCount;
};

uniform	sampler2D texUnit;

in vec3 Normal;
in vec2 TexCoord;
out vec4 outputF;

void main()
{
	vec4 color, emission,amb;
	float intensity;
	vec3 lightDir, n;
	
	lightDir = normalize(vec3(1.0,1.0,1.0));
	n = normalize(Normal);	
	intensity = max(dot(lightDir,n),0.0);
	
	if (texCount == 0) {
		color = diffuse;
		amb = ambient;
		emission = emissive;
	}
	else {
		color = texture(texUnit, TexCoord) * diffuse;
		amb = color * ambient;
		emission = texture(texUnit, TexCoord) * emissive;
	}
	outputF = (color * intensity) + amb + emission;
}
