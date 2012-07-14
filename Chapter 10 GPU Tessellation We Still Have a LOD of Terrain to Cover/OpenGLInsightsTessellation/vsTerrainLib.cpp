/** ----------------------------------------------------------
 * \class VSTerrainLib
 *
 * Lighthouse3D
 *
 * Very Simple Terrain Library
 *
 * \version 0.1.0
 * Initial Release
 *
 *  extends VSResourceLib 
 *
 * Full documentation at 
 * http://www.lighthouse3d.com/very-simple-libs
 *
 ---------------------------------------------------------------*/

#include "vsTerrainLib.h"

#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>



VSTerrainLib::VSTerrainLib():
		mGridSizeX(0),
		mGridSizeZ(0),
		mGridSpacing(1.0f),
		mHeightStep(0.0f),
		mScaleFactor(1)

{
	mVSML = VSMathLib::getInstance();
	mCenter[0] = 0.0f;
	mCenter[1] = 0.0f;
	mCenter[2] = 0.0f;

	for (int i = 0; i < VSShaderLib::MAX_TEXTURES; ++i) {
	
		mMesh.texUnits[i] = 0;
	}

	/* initialization of DevIL */
	ilInit(); 
}


VSTerrainLib::~VSTerrainLib() {

	// clear myMeshes stuff
			
	glDeleteVertexArrays(1,&(mMesh.vao));
	glDeleteBuffers(1,&(mMesh.uniformBlockIndex));
}




void 
VSTerrainLib::render () {

	mVSML->pushMatrix(VSMathLib::MODEL);
	//mVSML->scale(mGridSizeX*mGridSpacing, 1.0f, mGridSizeZ*mGridSpacing);
	//mVSML->translate(-mCenter[0], -mCenter[1], -mCenter[2]);

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
	glDrawElements(mMesh.type, mMesh.numIndices, GL_UNSIGNED_INT, 0);


	for (int t = 0; t < VSShaderLib::MAX_TEXTURES; t++) {
		if (mMesh.texUnits[t] != 0) {
			glActiveTexture(GL_TEXTURE0 + t);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}
	glBindVertexArray(0);
 
	mVSML->popMatrix(VSMathLib::MODEL);

}



// ----------------------------------------------------------------------------


bool
VSTerrainLib::load(std::string heightMap) 
{
	ILboolean success;
	GLuint buffer;

	unsigned int imageID;
	GLuint textureID;

	mScaleFactor = 1;

	ilGenImages(1, &imageID); 

	// Load Height Map
	ilBindImage(imageID); /* Binding of DevIL image name */
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT); 
	success = ilLoadImage((ILstring)heightMap.c_str());

	if (!success) {
		mLogError.addMessage("Couldn't load height map: %s", heightMap.c_str());
		/* The operation was not sucessfull hence free images and textures */
		glDeleteTextures(1, &textureID);
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

	mMesh.numIndices = (mGridSizeX -1 ) * (mGridSizeZ - 1) * 6;

	float *vertices = (float *)malloc(sizeof(float) * 4 * mGridSizeX * mGridSizeZ);

	unsigned int  *indices = (unsigned int *)malloc(sizeof(unsigned int) * mMesh.numIndices);

	for (unsigned int i = 0; i < mGridSizeX ; ++i) {
	
		for (unsigned int j = 0; j < mGridSizeZ; ++j) {
		
			vertices[(i * mGridSizeZ + j) * 4 + 0] = j / (1.0f * mGridSizeZ);
			vertices[(i * mGridSizeZ + j) * 4 + 1] = getHeight((int)j,(int)i) ;
			vertices[(i * mGridSizeZ + j) * 4 + 2] = i / (1.0f * mGridSizeX);
			vertices[(i * mGridSizeZ + j) * 4 + 3] = 1.0f;
		}
	}

	// set the indices array. For each cell create indices for 2 triangles
	for (unsigned int i = 0; i < mGridSizeX -1; ++i) {
		for (unsigned int j = 0; j < mGridSizeZ - 1; ++j) {
	
			indices[(i * (mGridSizeZ -1) + j) * 6 + 0] = i     * mGridSizeX + j;
			indices[(i * (mGridSizeZ -1) + j) * 6 + 1] = (i+1) * mGridSizeX + j;
			indices[(i * (mGridSizeZ -1) + j) * 6 + 2] = i     * mGridSizeX + j + 1;

			indices[(i * (mGridSizeZ -1) + j) * 6 + 3] = (i+1) * mGridSizeX + j;
			indices[(i * (mGridSizeZ -1) + j) * 6 + 4] = (i+1) * mGridSizeX + j + 1;
			indices[(i * (mGridSizeZ -1) + j) * 6 + 5] = i     * mGridSizeX + j + 1;
		}
	}

	unsigned int max = mGridSizeX > mGridSizeZ ? mGridSizeX : mGridSizeZ; 

	glGenVertexArrays(1, &(mMesh.vao));
	glBindVertexArray(mMesh.vao);

	//GLuint aux;
	//glGenBuffers(1,&aux);
	//glBindBuffer(GL_ARRAY_BUFFER, aux);
	//glBufferData(GL_ARRAY_BUFFER,
	//				sizeof(float) * 4 * mGridSizeX * mGridSizeZ,
	//				vertices,
	//				GL_STATIC_DRAW);

	//glGenBuffers(1,&aux);
	//glBindBuffer(GL_ARRAY_BUFFER, aux);
	//glBufferData(GL_ARRAY_BUFFER,
	//				sizeof(float) * 4 * mGridSizeX * mGridSizeZ,
	//				vertices,
	//				GL_STATIC_DRAW);
	//glGenBuffers(1,&aux);
	//glBindBuffer(GL_ARRAY_BUFFER, aux);
	//glBufferData(GL_ARRAY_BUFFER,
	//				sizeof(float) * 4 * mGridSizeX * mGridSizeZ,
	//				vertices,
	//				GL_STATIC_DRAW);
	//glGenBuffers(1,&aux);
	//glBindBuffer(GL_ARRAY_BUFFER, aux);
	//glBufferData(GL_ARRAY_BUFFER,
	//				sizeof(float) * 4 * mGridSizeX * mGridSizeZ,
	//				vertices,
	//				GL_STATIC_DRAW);
	// buffer for vertices
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER,
					sizeof(float) * 4 * mGridSizeX * mGridSizeZ,
					vertices,
					GL_STATIC_DRAW);
	glEnableVertexAttribArray(VSShaderLib::VERTEX_COORD_ATTRIB);
	glVertexAttribPointer(VSShaderLib::VERTEX_COORD_ATTRIB, 4, GL_FLOAT, 0, 0, 0);

	//buffer for indices
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
					sizeof(unsigned int) * mMesh.numIndices, 
					indices, 
					GL_STATIC_DRAW);

	mMesh.mat.ambient[0] = 0.2f;
	mMesh.mat.ambient[1] = 0.2f;
	mMesh.mat.ambient[2] = 0.2f;
	mMesh.mat.ambient[3] = 1.0f;

	mMesh.mat.diffuse[0] = 0.8f;
	mMesh.mat.diffuse[1] = 0.8f;
	mMesh.mat.diffuse[2] = 0.8f;
	mMesh.mat.diffuse[3] = 1.0f;

	mMesh.mat.texCount = 0;

	mMesh.type = GL_TRIANGLES;

	mCenter[0] = 0.5f;//(mGridSizeX * 0.5f * mGridSpacing);
	mCenter[1] = 0.0f;
	mCenter[2] = 0.5f;//(mGridSizeZ * 0.5f * mGridSpacing);

	// set identity transform
	mVSML->loadIdentity(VSMathLib::AUX0);
	mVSML->scale(VSMathLib::AUX0, mGridSizeX*mGridSpacing, 1.0f, mGridSizeZ*mGridSpacing);
	mVSML->translate(VSMathLib::AUX0, -mCenter[0], -mCenter[1], -mCenter[2]);

	memcpy(mMesh.transform, mVSML->get(VSMathLib::AUX0), sizeof(float) * 16);


//	mScaleFactor = 2.0f;///mGridSizeX ;

	ilDeleteImage(imageID);
	free (vertices);
	free (indices);

	setNormals();

	/*  */

	return true;
}


void 
VSTerrainLib::setTextureCoordinates(int mode) {

	GLuint buffer;
	float *tc = (float *)malloc(sizeof(float) * mGridSizeX * mGridSizeZ * 2);

	for (int i = 0; i < (int)mGridSizeX; ++i)
		for (int j = 0; j < (int)mGridSizeZ; ++j) {
		
			if (mode == 0) { // REPEAT texture in each grid square
				tc[(i*mGridSizeZ + j) * 2 + 0] = (float)( j);
				tc[(i*mGridSizeZ + j) * 2 + 1] = (float)( j);
			}
			else {
				tc[(i*mGridSizeZ + j) * 2 + 0] = j * 1.0f / mGridSizeZ;
				tc[(i*mGridSizeZ + j) * 2 + 1] = i * 1.0f / mGridSizeX;
			}		
		}

	glBindVertexArray(mMesh.vao);

	// buffer for textureCoordinates
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER,
					sizeof(float) * 2 * mGridSizeX * mGridSizeZ,
					tc,
					GL_STATIC_DRAW);
	glEnableVertexAttribArray(VSShaderLib::TEXTURE_COORD_ATTRIB);
	glVertexAttribPointer(VSShaderLib::TEXTURE_COORD_ATTRIB, 2, GL_FLOAT, 0, 0, 0);

	free (tc);
}


void 
VSTerrainLib::setNormals() {

	float deltaHi, deltaHj;
	float length;
	GLuint buffer;
	float *normals = (float *)malloc(sizeof(float) * mGridSizeX * mGridSizeZ * 3);
	float aux[3];

	for (int i = 0; i < (int)mGridSizeX; ++i)
		for (int j = 0; j < (int)mGridSizeZ; ++j) {
		
			deltaHj = getHeight(j, i+1) - getHeight(j, i-1);
			deltaHi = getHeight(j+1, i) - getHeight(j-1, i);

			aux[0] =  -2 * 1.0f/(mGridSizeX) * deltaHj;
			aux[1] =  4 * 1.0f/(mGridSizeX * mGridSizeZ);
			aux[2] =  2 * 1.0f/(mGridSizeZ) * deltaHi;
 		
			length = sqrt(aux[0] * aux[0] + aux[1] * aux[1] + aux[2] * aux[2]);

			if (deltaHi > 160)
				int x=0;

			aux[0] /= length;
			aux[1] /= length;
			aux[2] /= length;

			normals[(i*mGridSizeZ + j) * 3 + 0] = aux[0];
			normals[(i*mGridSizeZ + j) * 3 + 1] = aux[1];
			normals[(i*mGridSizeZ + j) * 3 + 2] = aux[2];
		}

	glBindVertexArray(mMesh.vao);

	// buffer for normals
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER,
					sizeof(float) * 3 * mGridSizeX * mGridSizeZ,
					normals,
					GL_STATIC_DRAW);
	glEnableVertexAttribArray(VSShaderLib::NORMAL_ATTRIB);
	glVertexAttribPointer(VSShaderLib::NORMAL_ATTRIB, 3, GL_FLOAT, 0, 0, 0);

	free (normals);
}


float 
VSTerrainLib::getHeight(int j , int i) {

	int iAux, jAux;

	iAux = (i / mScaleFactor);
	jAux = (j / mScaleFactor);

	if (iAux < 0) 
		iAux = 0;
	else if (iAux >= (int)(mGridSizeX / mScaleFactor))
		iAux = (mGridSizeX / mScaleFactor) - 1;

	if (jAux < 0) 
		jAux = 0;
	else if (jAux >= (int)(mGridSizeZ / mScaleFactor))
		jAux = (mGridSizeZ / (int)mScaleFactor) - 1;

	return(mHeights[(int)(iAux * mGridSizeX / mScaleFactor) + jAux] * mHeightStep / 65536 );

	//if (i < 0) 
	//	i = 0;
	//else if (i >= (int)(mGridSizeX / mScaleFactor))
	//	i = mGridSizeX - 1;

	//if (j < 0) 
	//	j = 0;
	//else if (j >= (int)(mGridSizeZ / mScaleFactor))
	//	j = mGridSizeZ - 1;

	//return(mHeights[i * mGridSizeX + j] * mHeightStep / 65536);
}


float 
VSTerrainLib::getHeight(float x, float z) {


	int x1, x2,z1,z2;
	float xGrid, zGrid;
	float fracX, fracZ, ha, hb;

	xGrid = x/mGridSpacing + mGridSizeX * 0.5f - 0.5f;
	zGrid = z/mGridSpacing + mGridSizeZ * 0.5f - 0.5f;

	x1 = (int)floor(xGrid);
	x2 = x1 + 1;
	fracX = (xGrid - x1);

	z1 = (int)floor(zGrid);
	z2 = z1 + 1;
	fracZ = (zGrid - z1);

	ha = fracX * getHeight(x2,z1) + (1-fracX) * getHeight(x1,z1);
	hb = fracX * getHeight(x2,z2) + (1-fracX) * getHeight(x1,z2);

	return(fracZ * hb + (ha * (1-fracZ)));
}


void
VSTerrainLib::addTexture(unsigned int unit, std::string filename) {

	mMesh.texUnits[unit] = loadRGBATexture(filename);
	mMesh.mat.texCount++;
}


void 
VSTerrainLib::setGridSpacing(float x) {

	mGridSpacing = x;
}


int
VSTerrainLib::getGridSize() {

	return mGridSizeX;
}


float 
VSTerrainLib::getGridSpacing() {

	return mGridSpacing;
}


void
VSTerrainLib::setHeightStep(float f) {

	mHeightStep = f;
}