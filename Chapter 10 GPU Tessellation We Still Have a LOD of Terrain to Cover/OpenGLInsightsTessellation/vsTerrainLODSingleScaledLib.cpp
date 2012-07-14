/** ----------------------------------------------------------
 * \class VSTerrainLODLib
 *
 * Lighthouse3D
 *
 * VSTerrainLODLib - Very Simple Terrain LOD Lib
 *
 * \version 0.1.0
 * Initial Release
 *
 * This lib provides an interface for Assimp to load and render 3D models
 *  and performs simple resource managment 
 *
 * Full documentation at 
 * http://www.lighthouse3d.com/very-simple-libs
 *
 ---------------------------------------------------------------*/

#include "vsTerrainLODSingleScaledLib.h"

#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define PATCHSIZE 64

VSTerrainLODSingleScaledLib::VSTerrainLODSingleScaledLib():
			VSTerrainLib()//, mScaleFactor(1)				
{
}


VSTerrainLODSingleScaledLib::~VSTerrainLODSingleScaledLib() {
			
	glDeleteVertexArrays(1,&(mMesh.vao));
//	glDeleteTextures(1,&(myMeshes[i].texIndex));
	glDeleteBuffers(1,&(mMesh.uniformBlockIndex));
}


//float 
//VSTerrainLODSingleScaledLib::getHeight(int j , int i) {
//
//	int iAux, jAux;
//
//	iAux = (i / mScaleFactor);
//	jAux = (j / mScaleFactor);
//
//	if (iAux < 0) 
//		iAux = 0;
//	else if (iAux >= (int)(mGridSizeX / mScaleFactor))
//		iAux = (mGridSizeX / mScaleFactor) - 1;
//
//	if (jAux < 0) 
//		jAux = 0;
//	else if (jAux >= (int)(mGridSizeZ / mScaleFactor))
//		jAux = (mGridSizeZ / (int)mScaleFactor) - 1;
//
//	return(mHeights[(int)(iAux * mGridSizeX / mScaleFactor) + jAux] * mHeightStep / 65536);
//}
//
//
//float 
//VSTerrainLODSingleScaledLib::getHeight(float x, float z) {
//
//
//	int x1, x2,z1,z2;
//	float xGrid, zGrid;
//	float fracX, fracZ, ha, hb;
//
//	xGrid = x/mGridSpacing + mGridSizeX * 0.5f - 0.5f;
//	zGrid = z/mGridSpacing + mGridSizeZ * 0.5f - 0.5f;
//
//	x1 = (int)floor(xGrid);
//	x2 = x1 + 1;
//	fracX = (xGrid - x1);
//
//	z1 = (int)floor(zGrid);
//	z2 = z1 + 1;
//	fracZ = (zGrid - z1);
//
//	ha = fracX * getHeight(x2,z1) + (1-fracX) * getHeight(x1,z1);
//	hb = fracX * getHeight(x2,z2) + (1-fracX) * getHeight(x1,z2);
//
//	return(fracZ * hb + (ha * (1-fracZ)));
//}


void 
VSTerrainLODSingleScaledLib::setScaleFactor(int f) {

	mScaleFactor = f;
}


int
VSTerrainLODSingleScaledLib::getScaleFactor() {

	return mScaleFactor;
}


void 
VSTerrainLODSingleScaledLib::render() {

	glPatchParameteri( GL_PATCH_VERTICES, 1 );

	mVSML->pushMatrix(VSMathLib::MODEL);
	mVSML->multMatrix(VSMathLib::MODEL, mMesh.transform);
	mVSML->matricesToGL();

	VSShaderLib::setBlock(mMaterialBlockName, &(mMesh.mat));
	// bind textures
	for (int t = 0; t < VSShaderLib::MAX_TEXTURES; t++) {
		if (mMesh.texUnits[t] != 0) {
			glActiveTexture(GL_TEXTURE0 + t);
			glBindTexture(GL_TEXTURE_2D, mMesh.texUnits[t]);
		}
	}
	// bind VAO
	glBindVertexArray(mMesh.vao);
	glDrawArrays(mMesh.type,0,mMesh.numIndices);

	for (int t = 0; t < VSShaderLib::MAX_TEXTURES; t++) {
		if (mMesh.texUnits[t] != 0) {
			glActiveTexture(GL_TEXTURE0 + t);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}
	glBindVertexArray(0);

	mVSML->popMatrix(VSMathLib::MODEL);
}


bool
VSTerrainLODSingleScaledLib::load(std::string heightMap) 
{
	ILboolean success;
	GLuint buffer;

	unsigned int imageID;
	GLuint textureID;

	ilGenImages(1, &imageID); 

	// Load Height Map
	ilBindImage(imageID); /* Binding of DevIL image name */
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT); 
	success = ilLoadImage((ILstring)heightMap.c_str());

	if (!success) {
		mLogError.addMessage("Couldn't load height map: %s", heightMap.c_str());
		/* The operation was not sucessfull hence free images */
		ilDeleteImages(1, &imageID); 
		return false;
	}

	/* Convert image to LUMINANCE - 16 bits */
	ilConvertImage(IL_LUMINANCE, IL_UNSIGNED_SHORT); 
	mGridSizeX = ilGetInteger(IL_IMAGE_WIDTH);
	mGridSizeZ = ilGetInteger(IL_IMAGE_HEIGHT);
	unsigned char *heightsAux = (unsigned char *)ilGetData();

	mHeights = (unsigned short *)malloc(sizeof(unsigned short) * mGridSizeX * mGridSizeZ);
	memcpy(mHeights, heightsAux, sizeof(unsigned short) * mGridSizeX * mGridSizeZ);

	if (mHeightStep == 0.0f)
		mHeightStep = 0.005f;

	/* Create and load heighmap texture to OpenGL */
	glGenTextures(1, &textureID); /* Texture name generation */
	glBindTexture(GL_TEXTURE_2D, textureID); 
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_R16,// GL_COMPRESSED_RED_RGTC1,
					mGridSizeX, mGridSizeZ, 
					0, GL_RED, GL_UNSIGNED_SHORT,
					mHeights); 
	
	glBindTexture(GL_TEXTURE_2D, 0);
	mGridSizeX = (int)(mGridSizeX * mScaleFactor);
	mGridSizeZ = (int)(mGridSizeZ * mScaleFactor);

	mMesh.texUnits[0] = textureID;

	if (mHeightStep == 0.0f)
		mHeightStep = 0.005f;

	int patchSize = 1;
	// We're assuming that the heightmap is a multiple of 64 and contains at least one patch
	int nopX = (mGridSizeX / PATCHSIZE);
	int nopZ = (mGridSizeZ / PATCHSIZE);
	int numPatches = nopX * nopZ;


	// Compute Roughness
	float difH;
	float *difs = (float *)malloc(sizeof(float) * nopX * nopZ);

	// First compute the average normal of the patch
	float avgN[3], n0[3], n1[3], n2[3], n3[3];
	float p0[3], p1[3], p2[3], p3[3];	
	float max=0.0f,min=1.0f;
	for (int i = 0; i < nopZ; ++i) {
			
		for (int j = 0; j < nopX; ++j) {

			p0[0] = 0.0f;
			p0[1] = getHeight(i*PATCHSIZE,j*PATCHSIZE);
			p0[2] = 0.0f;

			p1[0] = 0.0f;
			p1[1] = getHeight((i+1)*PATCHSIZE, j*PATCHSIZE);
			p1[2] = mGridSpacing*PATCHSIZE;

			p2[0] = mGridSpacing*PATCHSIZE;
			p2[1] = getHeight(i*PATCHSIZE,(j+1)*PATCHSIZE);
			p2[2] = 0.0f;

			p3[0] = mGridSpacing*PATCHSIZE;
			p3[1] = getHeight((i+1)*PATCHSIZE, (j+1)*PATCHSIZE);
			p3[2] = mGridSpacing*PATCHSIZE;

			float p01[3], p02[3], p13[3], p23[3];

			p01[0] = p1[0] - p0[0];
			p01[1] = p1[1] - p0[1];
			p01[2] = p1[2] - p0[2];

			p02[0] = p2[0] - p0[0];
			p02[1] = p2[1] - p0[1];
			p02[2] = p2[2] - p0[2];

			p13[0] = p3[0] - p1[0];
			p13[1] = p3[1] - p1[1];
			p13[2] = p3[2] - p1[2];

			p23[0] = p3[0] - p2[0];
			p23[1] = p3[1] - p2[1];
			p23[2] = p3[2] - p2[2];

			VSMathLib::crossProduct(p01,p02,n0);
			VSMathLib::crossProduct(p23,p02,n2);
			VSMathLib::crossProduct(p23,p13,n3);
			VSMathLib::crossProduct(p01,p13,n1);

			VSMathLib::normalize(n0);
			VSMathLib::normalize(n1);
			VSMathLib::normalize(n2);
			VSMathLib::normalize(n3);

			avgN[0] = n0[0] + n1[0] + n2[0] + n3[0];
			avgN[1] = n0[1] + n1[1] + n2[1] + n3[1];
			avgN[2] = n0[2] + n1[2] + n2[2] + n3[2];

			VSMathLib::normalize(avgN);

			// Then compute the normal at each point and accumulate its difference to the average

			difH = 0.0f;
			float normal[3], deltaHi, deltaHj;
			float maxDif = 0.0f, minDif = 1.0f;
			for (int k = 0; k < PATCHSIZE; ++k)
				for (int l = 0; l < PATCHSIZE; ++l) {
		
					deltaHi = getHeight(j*PATCHSIZE+k, i*PATCHSIZE+l+1) - getHeight(j*PATCHSIZE+k, i*PATCHSIZE+l-1);
					deltaHj = getHeight(j*PATCHSIZE+k+1, i*PATCHSIZE+l) - getHeight(j*PATCHSIZE+k-1, i*PATCHSIZE+l);

					normal[0] = -2 * mGridSpacing * deltaHi;
					normal[1] = 4 * mGridSpacing * mGridSpacing;
					normal[2] = - 2 * mGridSpacing * deltaHj;
 		
					VSMathLib::normalize(normal);
					float aux = VSMathLib::dotProduct(normal, avgN);
					aux *= aux;
					if (aux < minDif) minDif = aux;
					if (aux > maxDif) maxDif = aux;
					difH += aux;
					

				}
			difs[i*nopX + j] = minDif;//difH / (64*64);
			if (difs[i*nopX + j] > max) max = difs[i*nopX + j];
			if (difs[i*nopX + j] < min) min = difs[i*nopX + j];
		}
	}
//	printf("\n%f %f\n", min,max);
	glGenTextures(1, &textureID); /* Texture name generation */
	glBindTexture(GL_TEXTURE_2D, textureID); 
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, 
					nopX, nopZ, 
					0, GL_RED, GL_FLOAT,
					difs); 
	glBindTexture(GL_TEXTURE_2D, 0);
	mMesh.texUnits[1] = textureID;



	mMesh.numIndices = numPatches * patchSize;
	float *vertices = (float *)malloc(sizeof(float) * 2 * mMesh.numIndices);
	unsigned int  *indices = (unsigned int *)malloc(sizeof(unsigned int) * numPatches * patchSize);

	int patchNumber;
	for (int i = 0; i < nopX; ++i) {
	
		for (int j = 0; j < nopZ; ++j) {
		
			patchNumber = i * nopZ + j;

			vertices[(patchNumber * patchSize) * 2    ] = (i * PATCHSIZE ) * 1.0f / mGridSizeX ;
			vertices[(patchNumber * patchSize) * 2 + 1] = (j * PATCHSIZE) * 1.0f/ mGridSizeZ ;
					
		}
	}

	for (int i = 0; i < 12; ++i)
		printf("%f ", vertices[i]);

	for (int i = 0 ; i < mMesh.numIndices; ++i) {
		indices[i] = i;
	}

	glGenVertexArrays(1, &(mMesh.vao));
	glBindVertexArray(mMesh.vao);

	// buffer for vertices
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER,
					sizeof(float) * (mMesh.numIndices) * 2,
					vertices,
					GL_STATIC_DRAW);
	glEnableVertexAttribArray(VSShaderLib::VERTEX_COORD_ATTRIB);
	glVertexAttribPointer(VSShaderLib::VERTEX_COORD_ATTRIB, 2, GL_FLOAT, 0, 0, 0);

	mMesh.mat.ambient[0] = 0.2f;
	mMesh.mat.ambient[1] = 0.2f;
	mMesh.mat.ambient[2] = 0.2f;
	mMesh.mat.ambient[3] = 1.0f;

	mMesh.mat.diffuse[0] = 0.8f;
	mMesh.mat.diffuse[1] = 0.8f;
	mMesh.mat.diffuse[2] = 0.8f;
	mMesh.mat.diffuse[3] = 1.0f;

	mMesh.mat.texCount = 0;

	mMesh.type = GL_PATCHES;

	mCenter[0] =  (mGridSizeX * 0.5f * mGridSpacing);
	mCenter[1] = 0.0f;
	mCenter[2] =  (mGridSizeZ * 0.5f * mGridSpacing);

	// set identity transform
	mVSML->loadIdentity(VSMathLib::AUX0);
	mVSML->translate(VSMathLib::AUX0, 
				-mCenter[0], 0.0f, -mCenter[2]);
	memcpy(mMesh.transform, mVSML->get(VSMathLib::AUX0), sizeof(float) * 16);

	mCenter[0] = 0.5f;
	mCenter[1] = 0.0f;
	mCenter[2] = 0.5f;

//	pScaleFactor = 2.0f;

	/* Because we have already copied image data into texture data
	we can release memory used by image. */
	ilDeleteImages(1, &imageID); 
	free(vertices);
	free (indices);

	return true;
}

