#version 410

layout (std140) uniform Matrices {

	mat4 projMatrix;
	mat4 modelviewMatrix;
	mat4 projModelViewMatrix;
	mat3 normalMatrix;
};

uniform	sampler2D texUnit,heightMap;
uniform float heightStep;
uniform float gridSpacing;
uniform int scaleFactor;


in vec2 uvTE;

out vec4 outputF;


float height(float u, float v) {

	return (texture(heightMap, vec2(u,v)).r * heightStep);
}

void main() {

	vec4 color;
	float intensity;
	vec3 lightDir,n;
	
	float delta =  1.0 / (textureSize(heightMap,0).x * scaleFactor) ;

	vec3 deltaX = vec3(
				2.0 * gridSpacing,
				height(uvTE.s + delta, uvTE.t) - height(uvTE.s - delta, uvTE.t) , 
				0.0) ;
				
	vec3 deltaZ = vec3(
				0.0, 
				height(uvTE.s, uvTE.t + delta) - height(uvTE.s, uvTE.t - delta) , 
				2.0 * gridSpacing) ;
	
//	vec3 normalF = normalize(vec3(modelviewMatrix * vec4( cross(deltaZ, deltaX),0.0)));
	vec3 normalF = normalize(normalMatrix * cross(deltaZ, deltaX));
	lightDir = vec3(0.577,0.577,0.577);
	intensity = max(dot(lightDir,normalF),0.0);
	
	color = texture(texUnit, uvTE) * vec4(0.8, 0.8, 0.8, 1.0);
	color = color * intensity + color * vec4(0.2, 0.2, 0.2, 1.0);

	outputF =  color; 
}