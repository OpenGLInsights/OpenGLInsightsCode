#version 410


layout(vertices = 1) out;

layout (std140) uniform Matrices {

	mat4 projMatrix;
	mat4 modelviewMatrix;
	mat4 projModelViewMatrix;
	mat3 normalMatrix;
};

uniform float heightStep;
uniform float gridSpacing;
//uniform mat4 mvGroundCamMat;
uniform mat4 pvm;
uniform ivec2 viewportDim;
uniform sampler2D roughFactor;
uniform sampler2D heightMap;
uniform int useRoughness;
uniform int pixelsPerEdge;
uniform int culling;
uniform int scaleFactor;
uniform float patchSize;

in vec2 posV[];

out vec2 posTC[];

#define ID gl_InvocationID

float height(float u, float v) {

	return (texture(heightMap, vec2(u,v)).r  * heightStep);
}



// Checks if a segment is at least partially inside the frustum
// Need to add a little tolerance in here
bool segmentInFrustum(vec4 p1, vec4 p2) {

	if ((p1.x < -p1.w && p2.x < -p2.w) || (p1.x > p1.w && p2.x > p2.w) ||
//		(p1.y < -p1.w && p2.y < -p2.w) || (p1.y > p1.w && p2.y > p2.w) ||
		(p1.z < -p1.w && p2.z < -p2.w) || (p1.z > p1.w && p2.z > p2.w))
		return false;
	else
		return true;

}



// Measures the screen size of segment p1-p2
float screenSphereSize(vec4 p1, vec4 p2) {

	vec4 viewCenter = (p1+p2) * 0.5;
	vec4 viewUp = viewCenter;
	viewUp.y += distance(p1,p2);
	vec4 p1Proj = viewCenter;
	vec4 p2Proj = viewUp;
/*	vec4 p1Proj = projMatrix * viewCenter;
	vec4 p2Proj = projMatrix * viewUp;
*/	
	vec4 p1NDC, p2NDC;
	p1NDC = p1Proj/p1Proj.w;
	p2NDC = p2Proj/p2Proj.w;
	
	return( clamp(length((p2NDC.xy - p1NDC.xy) * viewportDim * 0.5) / (pixelsPerEdge), 1.0, patchSize));
}




float getRoughness(vec2 disp) {

	return (pow(( 1.8 - texture(roughFactor, posV[0]+ disp /textureSize(roughFactor,0)).x ),4));
//	return (texture(roughFactor, posV[0]+ disp /textureSize(roughFactor,0)).x );
}



void main() {

	int scaleF;
	if (scaleFactor == 0)
		scaleF = 1;
	else
		scaleF = scaleFactor;

	vec2 iLevel;
	vec4 oLevel;

	vec4 posTransV[4];
	vec2 pAux;
	vec2 posTCAux[4];

	ivec2 tSize = textureSize(heightMap,0) * scaleF;// * 2;
	float div = patchSize / tSize.x;

	posTC[gl_InvocationID] = posV[gl_InvocationID];

	
	posTCAux[0] = posV[0];
	posTCAux[1] = posV[0] + vec2(0.0, div);
	posTCAux[2] = posV[0] + vec2(div,0.0);
	posTCAux[3] = posV[0] + vec2(div,div);
	
/*	for (int i = 0; i < 4; ++i ) {
		pAux = posTCAux[i] * tSize * gridSpacing;
		posTransV[i] = pvm *vec4(pAux[0], height(posTCAux[i].x,posTCAux[i].y), pAux[1], 1.0);
	}
*/
	pAux = posTCAux[0] * tSize * gridSpacing;
	posTransV[0] = pvm * vec4(pAux[0], height(posTCAux[0].x,posTCAux[0].y), pAux[1], 1.0);

	pAux = posTCAux[1] * tSize * gridSpacing;
	posTransV[1] = pvm * vec4(pAux[0], height(posTCAux[1].x,posTCAux[1].y), pAux[1], 1.0);

	pAux = posTCAux[2] * tSize * gridSpacing;
	posTransV[2] = pvm * vec4(pAux[0], height(posTCAux[2].x,posTCAux[2].y), pAux[1], 1.0);

	pAux = posTCAux[3] * tSize * gridSpacing;
	posTransV[3] = pvm * vec4(pAux[0], height(posTCAux[3].x,posTCAux[3].y), pAux[1], 1.0);

/*	pAux = posTCAux[0] * tSize * gridSpacing;
	posTransV[0] = mvGroundCamMat * vec4(pAux[0], height(posTCAux[0].x,posTCAux[0].y), pAux[1], 1.0);

	pAux = posTCAux[1] * tSize * gridSpacing;
	posTransV[1] = mvGroundCamMat * vec4(pAux[0], height(posTCAux[1].x,posTCAux[1].y), pAux[1], 1.0);

	pAux = posTCAux[2] * tSize * gridSpacing;
	posTransV[2] = mvGroundCamMat * vec4(pAux[0], height(posTCAux[2].x,posTCAux[2].y), pAux[1], 1.0);

	pAux = posTCAux[3] * tSize * gridSpacing;
	posTransV[3] = mvGroundCamMat * vec4(pAux[0], height(posTCAux[3].x,posTCAux[3].y), pAux[1], 1.0);
*/
		
	if (culling == 0 ||(		
	            segmentInFrustum(posTransV[ID], posTransV[ID+1]) ||
				segmentInFrustum(posTransV[ID], posTransV[ID+2]) ||
				segmentInFrustum(posTransV[ID+2], posTransV[ID+3]) ||
				segmentInFrustum(posTransV[ID+3], posTransV[ID+1]))) {
					
		if (useRoughness == 1) {
			float roughness[4];
			float roughnessForCentralPatch = getRoughness(vec2(0.5));
			roughness[0] = max(roughnessForCentralPatch, getRoughness(vec2(-0.5,0.5)));
			roughness[1] = max(roughnessForCentralPatch, getRoughness(vec2(0.5,-0.5)));
			roughness[2] = max(roughnessForCentralPatch, getRoughness(vec2(1.5,0.5)));
			roughness[3] = max(roughnessForCentralPatch, getRoughness(vec2(0.5,1.5)));
			oLevel =  
						vec4(clamp(screenSphereSize(posTransV[ID], posTransV[ID+1]) * roughness[0],1,patchSize), 
								clamp(screenSphereSize(posTransV[ID+0], posTransV[ID+2]) * roughness[1],1,patchSize),
								clamp(screenSphereSize(posTransV[ID+2], posTransV[ID+3]) * roughness[2],1,patchSize),
								clamp(screenSphereSize(posTransV[ID+3], posTransV[ID+1]) * roughness[3],1,patchSize)) ;
//			oLevel = clamp(oLevel, vec4(1),vec4(patchSize));
			iLevel = vec2(max(oLevel[1] , oLevel[3]) , max(oLevel[0] , oLevel[2]) );
		}
		else if (useRoughness == 0) {
					
		// Screen size based LOD

			oLevel = vec4(screenSphereSize(posTransV[ID], posTransV[ID+1]),
						screenSphereSize(posTransV[ID+0], posTransV[ID+2]),
						screenSphereSize(posTransV[ID+2], posTransV[ID+3]),
						screenSphereSize(posTransV[ID+3], posTransV[ID+1]));
			iLevel = vec2(max(oLevel[1] , oLevel[3]) , max(oLevel[0] , oLevel[2]) );
			
		}
		else {
			oLevel = vec4(patchSize);
			iLevel = vec2(patchSize);
		
		}
	}
	else if (culling == 1) {
		oLevel = vec4(0);
		iLevel = vec2(0);
		
	}
	else {
		oLevel = vec4(patchSize);
		iLevel = vec2(patchSize);
		
	}

	gl_TessLevelOuter[0] = oLevel[0];
	gl_TessLevelOuter[1] = oLevel[1];
	gl_TessLevelOuter[2] = oLevel[2];
	gl_TessLevelOuter[3] = oLevel[3];
	gl_TessLevelInner[0] = iLevel[0];
	gl_TessLevelInner[1] = iLevel[1];
}