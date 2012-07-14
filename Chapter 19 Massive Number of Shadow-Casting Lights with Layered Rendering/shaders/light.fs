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

layout(std140, binding = 2) uniform lightTransform {
	mat4 VPMatrix[600];
} LightTransform;

layout(binding = 0) uniform sampler2D smpPositionBuffer;
layout(binding = 1) uniform sampler2D smpNormalBuffer;
layout(binding = 2) uniform sampler2D smpDepthBuffer;
layout(binding = 3) uniform sampler2DArrayShadow smpShadowMapArray;

layout(location = 0) in vec2 interpTexCoord;
layout(location = 1) in vec3 lightPosition;
layout(location = 2) in vec4 lightColor;
layout(location = 3) flat in int lightID;

layout(location = 0) out vec4 outFragmentColor;

void main(void) {

	/* reconstruct normal from X and Y component (not 100% accurate though
	   as Z can be sometimes negative due to perspective projection) */
	vec3 normal = texture(smpNormalBuffer, interpTexCoord).xyz;
	
	/* get position */
	vec4 position = texture(smpPositionBuffer, interpTexCoord);
	
	/* calculate attenuation */
	vec3 lightVec = lightPosition - position.xyz;
	float sqrDist = dot(lightVec, lightVec);
	float att = 1.f / (1.f + 0.01f * sqrDist);
	
	/* calculate diffuse */
	float diffuse = clamp(dot(normal, normalize(lightVec)), 0.f, 1.f);
	
	/* calculate projected shadow map coordinates */
	vec4 shadowCoordHom = LightTransform.VPMatrix[lightID] * position;
	
	/* we manually have to do the perspective divide as there is no
	   version of textureProj that can take a sampler2DArrayShadow */
	shadowCoordHom.xyz /= shadowCoordHom.w;
	shadowCoordHom.xyz = shadowCoordHom.xyz * 0.5 + 0.5;
	shadowCoordHom.z *= 0.9999;
	vec4 shadowCoord = vec4(shadowCoordHom.x, shadowCoordHom.y, lightID, shadowCoordHom.z);
	
	/* calculate shadow factor */
	float shadow = texture(smpShadowMapArray, shadowCoord);
	
	/* apply counter shadow eliminator factor */
	float counterShadow = min(floor(shadowCoord.w), 1.f);
	shadow = mix(shadow, 1.f, counterShadow);

	outFragmentColor = lightColor * att * diffuse * shadow;
	
}
