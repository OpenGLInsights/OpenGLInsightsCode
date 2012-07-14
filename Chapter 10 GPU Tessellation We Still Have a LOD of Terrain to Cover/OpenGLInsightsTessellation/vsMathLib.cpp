/* --------------------------------------------------

Lighthouse3D

VSMathLib - Very Simple Matrix Library

http://www.lighthouse3d.com/very-simple-libs

----------------------------------------------------*/

#include "vsMathLib.h"
#include "vsShaderLib.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// This var keeps track of the single instance of VSMathLib
VSMathLib* VSMathLib::gInstance = 0;

#ifdef _WIN32
#define M_PI       3.14159265358979323846f
#endif

static inline float 
DegToRad(float degrees) 
{ 
	return (float)(degrees * (M_PI / 180.0f));
};


// Singleton implementation
// use this function to get the instance of VSMathLib
VSMathLib*
VSMathLib::getInstance (void) {
	
	if (0 != gInstance) 
		return gInstance;
	else
		gInstance = new VSMathLib();

	return gInstance;
}


// VSMathLib constructor
VSMathLib::VSMathLib():
		mInit(false),
		mBlocks(false)
{
	// set all uniform names to ""
	for (int i = 0; i < COUNT_MATRICES; ++i) {
		mUniformName[i] = "";
		mUniformArrayIndex[i] = 0;
	}
	for (int i = 0; i < COUNT_COMPUTED_MATRICES; ++i) {
		mComputedMatUniformName[i] = "";
		mComputedMatUniformArrayIndex[i] = 0;
	}
}


// VSMathLib destructor		
VSMathLib::~VSMathLib()
{
}


void 
VSMathLib::setUniformBlockName(std::string blockName) {

	mInit = true;
	// We ARE using blocks
	mBlocks = true;
	mBlockName = blockName;
}
		

void 
VSMathLib::setUniformName(MatrixTypes matType, std::string uniformName) {

	mInit = true;
	mUniformName[matType] = uniformName;
	mUniformArrayIndex[matType] = 0;
}


void 
VSMathLib::setUniformName(ComputedMatrixTypes matType, std::string uniformName) {

	mInit = true;
	mComputedMatUniformName[matType] = uniformName;
	mComputedMatUniformArrayIndex[matType] = 0;
}


void 
VSMathLib::setUniformArrayIndexName(MatrixTypes matType, 
							std::string uniformName, int index) {

	mInit = true;
	mUniformName[matType] = uniformName;
	mUniformArrayIndex[matType] = index;
}


void 
VSMathLib::setUniformArrayIndexName(ComputedMatrixTypes matType, 
							std::string uniformName, int index) {

	mInit = true;
	mComputedMatUniformName[matType] = uniformName;
	mComputedMatUniformArrayIndex[matType] = index;
}


// glPushMatrix implementation
void 
VSMathLib::pushMatrix(MatrixTypes aType) {

	float *aux = (float *)malloc(sizeof(float) * 16);
	memcpy(aux, mMatrix[aType], sizeof(float) * 16);
	mMatrixStack[aType].push_back(aux);
}


// glPopMatrix implementation
void 
VSMathLib::popMatrix(MatrixTypes aType) {

	float *m = mMatrixStack[aType][mMatrixStack[aType].size()-1];
	memcpy(mMatrix[aType], m, sizeof(float) * 16);
	mMatrixStack[aType].pop_back();
	free(m);
}


// glLoadIdentity implementation
void 
VSMathLib::loadIdentity(MatrixTypes aType)
{
	setIdentityMatrix(mMatrix[aType]);
}


// glMultMatrix implementation
void 
VSMathLib::multMatrix(MatrixTypes aType, float *aMatrix)
{
	
	float *a, *b, res[16];
	a = mMatrix[aType];
	b = aMatrix;

	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			res[j*4 + i] = 0.0f;
			for (int k = 0; k < 4; ++k) {
				res[j*4 + i] += a[k*4 + i] * b[j*4 + k]; 
			}
		}
	}
	memcpy(mMatrix[aType], res, 16 * sizeof(float));
}




// glLoadMatrix implementation
void 
VSMathLib::loadMatrix(MatrixTypes aType, float *aMatrix)
{
	memcpy(mMatrix[aType], aMatrix, 16 * sizeof(float));
}


// glTranslate implementation with matrix selection
void 
VSMathLib::translate(MatrixTypes aType, float x, float y, float z) 
{
	float mat[16];

	setIdentityMatrix(mat);
	mat[12] = x;
	mat[13] = y;
	mat[14] = z;

	multMatrix(aType,mat);
}


// glTranslate on the MODEL matrix
void 
VSMathLib::translate(float x, float y, float z) 
{
	translate(MODEL, x,y,z);
}


// glScale implementation with matrix selection
void 
VSMathLib::scale(MatrixTypes aType, float x, float y, float z) 
{
	float mat[16];

	setIdentityMatrix(mat,4);
	mat[0] = x;
	mat[5] = y;
	mat[10] = z;

	multMatrix(aType,mat);
}


// glScale on the MODELVIEW matrix
void 
VSMathLib::scale(float x, float y, float z) 
{
	scale(MODEL, x, y, z);
}


// glRotate implementation with matrix selection
void 
VSMathLib::rotate(MatrixTypes aType, float angle, float x, float y, float z)
{
	float mat[16];

	float radAngle = DegToRad(angle);
	float co = cos(radAngle);
	float si = sin(radAngle);
	float x2 = x*x;
	float y2 = y*y;
	float z2 = z*z;

	mat[0] = x2 + (y2 + z2) * co; 
	mat[4] = x * y * (1 - co) - z * si;
	mat[8] = x * z * (1 - co) + y * si;
	mat[12]= 0.0f;
	   
	mat[1] = x * y * (1 - co) + z * si;
	mat[5] = y2 + (x2 + z2) * co;
	mat[9] = y * z * (1 - co) - x * si;
	mat[13]= 0.0f;
	   
	mat[2] = x * z * (1 - co) - y * si;
	mat[6] = y * z * (1 - co) + x * si;
	mat[10]= z2 + (x2 + y2) * co;
	mat[14]= 0.0f;
	   
	mat[3] = 0.0f;
	mat[7] = 0.0f;
	mat[11]= 0.0f;
	mat[15]= 1.0f;

	multMatrix(aType,mat);
}


// glRotate implementation in the MODELVIEW matrix
void 
VSMathLib::rotate(float angle, float x, float y, float z)
{
	rotate(MODEL,angle,x,y,z);
}


// gluLookAt implementation
void 
VSMathLib::lookAt(float xPos, float yPos, float zPos,
					float xLook, float yLook, float zLook,
					float xUp, float yUp, float zUp)
{
	float dir[3], right[3], up[3];

	up[0] = xUp;	up[1] = yUp;	up[2] = zUp;

	dir[0] =  (xLook - xPos);
	dir[1] =  (yLook - yPos);
	dir[2] =  (zLook - zPos);
	normalize(dir);

	crossProduct(dir,up,right);
	normalize(right);

	crossProduct(right,dir,up);
	normalize(up);

	float m1[16],m2[16];

	m1[0]  = right[0];
	m1[4]  = right[1];
	m1[8]  = right[2];
	m1[12] = 0.0f;

	m1[1]  = up[0];
	m1[5]  = up[1];
	m1[9]  = up[2];
	m1[13] = 0.0f;

	m1[2]  = -dir[0];
	m1[6]  = -dir[1];
	m1[10] = -dir[2];
	m1[14] =  0.0f;

	m1[3]  = 0.0f;
	m1[7]  = 0.0f;
	m1[11] = 0.0f;
	m1[15] = 1.0f;

	setIdentityMatrix(m2,4);
	m2[12] = -xPos;
	m2[13] = -yPos;
	m2[14] = -zPos;

	multMatrix(VIEW, m1);
	multMatrix(VIEW, m2);
}


// gluPerspective implementation
void 
VSMathLib::perspective(float fov, float ratio, float nearp, float farp)
{
	float projMatrix[16];

	float f = 1.0f / tan (fov * (M_PI / 360.0f));

	setIdentityMatrix(projMatrix,4);

	projMatrix[0] = f / ratio;
	projMatrix[1 * 4 + 1] = f;
	projMatrix[2 * 4 + 2] = (farp + nearp) / (nearp - farp);
	projMatrix[3 * 4 + 2] = (2.0f * farp * nearp) / (nearp - farp);
	projMatrix[2 * 4 + 3] = -1.0f;
	projMatrix[3 * 4 + 3] = 0.0f;

	multMatrix(PROJECTION, projMatrix);
}


// glOrtho implementation
void 
VSMathLib::ortho(float left, float right, 
			float bottom, float top, 
			float nearp, float farp)
{
	float m[16];

	setIdentityMatrix(m,4);

	m[0 * 4 + 0] = 2 / (right - left);
	m[1 * 4 + 1] = 2 / (top - bottom);	
	m[2 * 4 + 2] = -2 / (farp - nearp);
	m[3 * 4 + 0] = -(right + left) / (right - left);
	m[3 * 4 + 1] = -(top + bottom) / (top - bottom);
	m[3 * 4 + 2] = -(farp + nearp) / (farp - nearp);

	multMatrix(PROJECTION, m);
}


// glFrustum implementation
void 
VSMathLib::frustum(float left, float right, 
			float bottom, float top, 
			float nearp, float farp)
{
	float m[16];

	setIdentityMatrix(m,4);

	m[0 * 4 + 0] = 2 * nearp / (right-left);
	m[1 * 4 + 1] = 2 * nearp / (top - bottom);
	m[2 * 4 + 0] = (right + left) / (right - left);
	m[2 * 4 + 1] = (top + bottom) / (top - bottom);
	m[2 * 4 + 2] = - (farp + nearp) / (farp - nearp);
	m[2 * 4 + 3] = -1.0f;
	m[3 * 4 + 2] = - 2 * farp * nearp / (farp-nearp);
	m[3 * 4 + 3] = 0.0f;

	multMatrix(PROJECTION, m);
}


// returns a pointer to the requested matrix
float *
VSMathLib::get(MatrixTypes aType)
{
	return mMatrix[aType];
}


// returns a pointer to the requested matrix
float *
VSMathLib::get(ComputedMatrixTypes aType)
{
	switch (aType) {
	
		case NORMAL: 
			computeNormalMatrix3x3();
			return mNormal3x3; 
			break;
		default:
			computeDerivedMatrix(aType);
			return mCompMatrix[aType]; 
			break;
	}
	// this should never happen!
	return NULL;
}

/* -----------------------------------------------------
             SEND MATRICES TO OPENGL
------------------------------------------------------*/




// universal
void
VSMathLib::matrixToGL(MatrixTypes aType)
{
	if (mInit) {
	
		if (mBlocks) {
			if (mUniformName[aType] != "") {
				if (mUniformArrayIndex[aType]) {
					VSShaderLib::setBlockUniformArrayElement(mBlockName, 
														mUniformName[aType],
														mUniformArrayIndex[aType],
														mMatrix[aType]);
				}
				else {
					VSShaderLib::setBlockUniform(mBlockName, 
											mUniformName[aType], 
											mMatrix[aType]);
				}
			}
		}
		else {
			int p,loc;
			if (mUniformName[aType] != "") {
				glGetIntegerv(GL_CURRENT_PROGRAM,&p);
				loc = glGetUniformLocation(p, mUniformName[aType].c_str());
				glProgramUniformMatrix4fv(p, loc, 1, false, mMatrix[aType]);
			}
		}
	
	}
}

void
VSMathLib::matrixToGL(ComputedMatrixTypes aType)
{
	if (mInit) {
	
		if (mBlocks) {
			if (aType == NORMAL && mComputedMatUniformName[NORMAL] != "") {
				computeNormalMatrix();
				if (mComputedMatUniformArrayIndex[NORMAL])
					VSShaderLib::setBlockUniformArrayElement(mBlockName, 
									mComputedMatUniformName[NORMAL], 
									mComputedMatUniformArrayIndex[NORMAL],
									mNormal);
				else
					VSShaderLib::setBlockUniform(mBlockName, 
									mComputedMatUniformName[NORMAL],
									mNormal);
			}
			else if (mComputedMatUniformName[aType] != "") {
				computeDerivedMatrix(aType);
				if (mComputedMatUniformArrayIndex[aType])
					VSShaderLib::setBlockUniformArrayElement(mBlockName, 
									mComputedMatUniformName[aType], 
									mComputedMatUniformArrayIndex[aType],
									mCompMatrix[aType]);
				else
					VSShaderLib::setBlockUniform(mBlockName, 
									mComputedMatUniformName[aType], 
									mCompMatrix[aType]);
			}
			}
		}
		else {
			int p,loc;
			if (mUniformName[aType] != "") {
				glGetIntegerv(GL_CURRENT_PROGRAM,&p);
				loc = glGetUniformLocation(p, mUniformName[aType].c_str());
			if (aType == NORMAL && mComputedMatUniformName[NORMAL] != "") {
				computeNormalMatrix3x3();
				loc = glGetUniformLocation(p, 
									mComputedMatUniformName[NORMAL].c_str());
				glProgramUniformMatrix3fv(p, loc, 1, false, mNormal3x3);
			}
			else if (mComputedMatUniformName[aType] != "") {
				computeDerivedMatrix(aType);
				loc = glGetUniformLocation(p, 
									mComputedMatUniformName[aType].c_str());
				glProgramUniformMatrix4fv(p, loc, 1, false, mCompMatrix[aType]);
			
			}
		}
	
	}
}

// Sends all matrices whose respectived uniforms have been named
void
VSMathLib::matricesToGL() {

	if (mInit) {
	
		if (mBlocks) {
			for (int i = 0 ; i < COUNT_MATRICES; ++i ) {
				if (mUniformName[i] != "")
					if (mUniformArrayIndex[i])
						VSShaderLib::setBlockUniformArrayElement(mBlockName, 
															mUniformName[i],
															mUniformArrayIndex[i],
															mMatrix[i]);
					else
						VSShaderLib::setBlockUniform(mBlockName, mUniformName[i], mMatrix[i]);
			}
			if (mComputedMatUniformName[NORMAL] != "") {
				computeNormalMatrix();
				if (mComputedMatUniformArrayIndex[NORMAL])
					VSShaderLib::setBlockUniformArrayElement(mBlockName, 
									mComputedMatUniformName[NORMAL], 
									mComputedMatUniformArrayIndex[NORMAL],
									mNormal);
				else
					VSShaderLib::setBlockUniform(mBlockName, 
									mComputedMatUniformName[NORMAL],
									mNormal);
			}

			for (int i = 0; i < COUNT_COMPUTED_MATRICES-1; ++i) {

				if (mComputedMatUniformName[i] != "") {
					computeDerivedMatrix((VSMathLib::ComputedMatrixTypes)i);
					if (mComputedMatUniformArrayIndex[i])
						VSShaderLib::setBlockUniformArrayElement(mBlockName, 
										mComputedMatUniformName[i], 
										mComputedMatUniformArrayIndex[i],
										mCompMatrix[i]);
					else
					VSShaderLib::setBlockUniform(mBlockName, 
										mComputedMatUniformName[i], 
										mCompMatrix[i]);
				}
			}
		}
		else {
			int p,loc;
			glGetIntegerv(GL_CURRENT_PROGRAM,&p);
			for (int i = 0; i < COUNT_MATRICES; ++i) {
				if (mUniformName[i] != "") {
					loc = glGetUniformLocation(p, mUniformName[i].c_str());
					glProgramUniformMatrix4fv(p, loc, 1, false, mMatrix[i]);
				}
			}
			if (mComputedMatUniformName[NORMAL] != "") {
				computeNormalMatrix3x3();
				loc = glGetUniformLocation(p, 
									mComputedMatUniformName[NORMAL].c_str());

				glProgramUniformMatrix3fv(p, loc, 1, false, mNormal3x3);
			}
			for (int i = 0; i < COUNT_COMPUTED_MATRICES-1; ++i) {
				if (mComputedMatUniformName[i] != "") {
					computeDerivedMatrix((VSMathLib::ComputedMatrixTypes)i);
					loc = glGetUniformLocation(p, 
									mComputedMatUniformName[i].c_str());
					glProgramUniformMatrix4fv(p, loc, 1, false, mCompMatrix[i]);
			}
			}
		}
	
	}
}

// -----------------------------------------------------
//                      AUX functions
// -----------------------------------------------------

// sets the square matrix mat to the identity matrix,
// size refers to the number of rows (or columns)
void 
VSMathLib::setIdentityMatrix( float *mat, int size) {

	// fill matrix with 0s
	for (int i = 0; i < size * size; ++i)
			mat[i] = 0.0f;

	// fill diagonal with 1s
	for (int i = 0; i < size; ++i)
		mat[i + i * size] = 1.0f;
}


// Compute res = M * point
void
VSMathLib::multMatrixPoint(MatrixTypes aType, float *point, float *res) {

	for (int i = 0; i < 4; ++i) {

		res[i] = 0.0f;
	
		for (int j = 0; j < 4; j++) {
		
			res[i] += point[j] * mMatrix[aType][j*4 + i];
		} 
	}
}

// Compute res = M * point
void
VSMathLib::multMatrixPoint(ComputedMatrixTypes aType, float *point, float *res) {


	if (aType == NORMAL) {
		computeNormalMatrix();
		for (int i = 0; i < 3; ++i) {

			res[i] = 0.0f;
	
			for (int j = 0; j < 3; j++) {
		
				res[i] += point[j] * mNormal[j*4 + i];
			} 
		}
	}

	else {
		computeDerivedMatrix(aType);
		for (int i = 0; i < 4; ++i) {

			res[i] = 0.0f;
	
			for (int j = 0; j < 4; j++) {
		
				res[i] += point[j] * mCompMatrix[aType][j*4 + i];
			} 
		}
	}
}

// res = a cross b;
void 
VSMathLib::crossProduct( float *a, float *b, float *res) {

	res[0] = a[1] * b[2]  -  b[1] * a[2];
	res[1] = a[2] * b[0]  -  b[2] * a[0];
	res[2] = a[0] * b[1]  -  b[0] * a[1];
}


// returns a . b
float
VSMathLib::dotProduct(float *a, float *b) {

	float res = a[0] * b[0]  +  a[1] * b[1]  +  a[2] * b[2];

	return res;
}


// Normalize a vec3
void 
VSMathLib::normalize(float *a) {

	float mag = sqrt(a[0] * a[0]  +  a[1] * a[1]  +  a[2] * a[2]);

	a[0] /= mag;
	a[1] /= mag;
	a[2] /= mag;
}


// res = a - b
void
VSMathLib::subtract(float *a, float *b, float *res) {

	res[0] = b[0] - a[0];
	res[1] = b[1] - a[1];
	res[2] = b[2] - a[2];
}


// res = a + b
void
VSMathLib::add(float *a, float *b, float *res) {

	res[0] = b[0] + a[0];
	res[1] = b[1] + a[1];
	res[2] = b[2] + a[2];
}


// returns |a|
float
VSMathLib::length(float *a) {

	return(sqrt(a[0] * a[0]  +  a[1] * a[1]  +  a[2] * a[2]));

}


static inline int 
M3(int i, int j)
{ 
   return (i*3+j); 
};


// computes the derived normal matrix
void
VSMathLib::computeNormalMatrix() {

	computeDerivedMatrix(VIEW_MODEL);

	mMat3x3[0] = mCompMatrix[VIEW_MODEL][0];
	mMat3x3[1] = mCompMatrix[VIEW_MODEL][1];
	mMat3x3[2] = mCompMatrix[VIEW_MODEL][2];

	mMat3x3[3] = mCompMatrix[VIEW_MODEL][4];
	mMat3x3[4] = mCompMatrix[VIEW_MODEL][5];
	mMat3x3[5] = mCompMatrix[VIEW_MODEL][6];

	mMat3x3[6] = mCompMatrix[VIEW_MODEL][8];
	mMat3x3[7] = mCompMatrix[VIEW_MODEL][9];
	mMat3x3[8] = mCompMatrix[VIEW_MODEL][10];

	float det, invDet;

	det = mMat3x3[0] * (mMat3x3[4] * mMat3x3[8] - mMat3x3[5] * mMat3x3[7]) +
		  mMat3x3[1] * (mMat3x3[5] * mMat3x3[6] - mMat3x3[8] * mMat3x3[3]) +
		  mMat3x3[2] * (mMat3x3[3] * mMat3x3[7] - mMat3x3[4] * mMat3x3[6]);

	invDet = 1.0f/det;

	mNormal[0] = (mMat3x3[4] * mMat3x3[8] - mMat3x3[5] * mMat3x3[7]) * invDet;
	mNormal[1] = (mMat3x3[5] * mMat3x3[6] - mMat3x3[8] * mMat3x3[3]) * invDet;
	mNormal[2] = (mMat3x3[3] * mMat3x3[7] - mMat3x3[4] * mMat3x3[6]) * invDet;
	mNormal[3] = 0.0f;
	mNormal[4] = (mMat3x3[2] * mMat3x3[7] - mMat3x3[1] * mMat3x3[8]) * invDet;
	mNormal[5] = (mMat3x3[0] * mMat3x3[8] - mMat3x3[2] * mMat3x3[6]) * invDet;
	mNormal[6] = (mMat3x3[1] * mMat3x3[6] - mMat3x3[7] * mMat3x3[0]) * invDet;
	mNormal[7] = 0.0f;
	mNormal[8] = (mMat3x3[1] * mMat3x3[5] - mMat3x3[4] * mMat3x3[2]) * invDet;
	mNormal[9] = (mMat3x3[2] * mMat3x3[3] - mMat3x3[0] * mMat3x3[5]) * invDet;
	mNormal[10] =(mMat3x3[0] * mMat3x3[4] - mMat3x3[3] * mMat3x3[1]) * invDet;
	mNormal[11] = 0.0;

}


// computes the derived normal matrix
void
VSMathLib::computeNormalMatrix3x3() {

	computeDerivedMatrix(VIEW_MODEL);

	mMat3x3[0] = mCompMatrix[VIEW_MODEL][0];
	mMat3x3[1] = mCompMatrix[VIEW_MODEL][1];
	mMat3x3[2] = mCompMatrix[VIEW_MODEL][2];

	mMat3x3[3] = mCompMatrix[VIEW_MODEL][4];
	mMat3x3[4] = mCompMatrix[VIEW_MODEL][5];
	mMat3x3[5] = mCompMatrix[VIEW_MODEL][6];

	mMat3x3[6] = mCompMatrix[VIEW_MODEL][8];
	mMat3x3[7] = mCompMatrix[VIEW_MODEL][9];
	mMat3x3[8] = mCompMatrix[VIEW_MODEL][10];

	float det, invDet;

	det = mMat3x3[0] * (mMat3x3[4] * mMat3x3[8] - mMat3x3[5] * mMat3x3[7]) +
		  mMat3x3[1] * (mMat3x3[5] * mMat3x3[6] - mMat3x3[8] * mMat3x3[3]) +
		  mMat3x3[2] * (mMat3x3[3] * mMat3x3[7] - mMat3x3[4] * mMat3x3[6]);

	invDet = 1.0f/det;

	mNormal3x3[0] = (mMat3x3[4] * mMat3x3[8] - mMat3x3[5] * mMat3x3[7]) * invDet;
	mNormal3x3[1] = (mMat3x3[5] * mMat3x3[6] - mMat3x3[8] * mMat3x3[3]) * invDet;
	mNormal3x3[2] = (mMat3x3[3] * mMat3x3[7] - mMat3x3[4] * mMat3x3[6]) * invDet;
	mNormal3x3[3] = (mMat3x3[2] * mMat3x3[7] - mMat3x3[1] * mMat3x3[8]) * invDet;
	mNormal3x3[4] = (mMat3x3[0] * mMat3x3[8] - mMat3x3[2] * mMat3x3[6]) * invDet;
	mNormal3x3[5] = (mMat3x3[1] * mMat3x3[6] - mMat3x3[7] * mMat3x3[0]) * invDet;
	mNormal3x3[6] = (mMat3x3[1] * mMat3x3[5] - mMat3x3[4] * mMat3x3[2]) * invDet;
	mNormal3x3[7] = (mMat3x3[2] * mMat3x3[3] - mMat3x3[0] * mMat3x3[5]) * invDet;
	mNormal3x3[8] = (mMat3x3[0] * mMat3x3[4] - mMat3x3[3] * mMat3x3[1]) * invDet;

}


// Computes derived matrices
void 
VSMathLib::computeDerivedMatrix(ComputedMatrixTypes aType) {
	
	memcpy(mCompMatrix[VIEW_MODEL], mMatrix[VIEW], 16 * sizeof(float));
	multMatrix(mCompMatrix[VIEW_MODEL], mMatrix[MODEL]);

	if (aType == PROJ_VIEW_MODEL) {
		memcpy(mCompMatrix[PROJ_VIEW_MODEL], mMatrix[PROJECTION], 16 * sizeof(float));
		multMatrix(mCompMatrix[PROJ_VIEW_MODEL], mCompMatrix[VIEW_MODEL]);
	}
}


// aux function resMat = resMat * aMatrix
void 
VSMathLib::multMatrix(float *resMat, float *aMatrix)
{
	
	float *a, *b, res[16];
	a = resMat;
	b = aMatrix;

	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			res[j*4 + i] = 0.0f;
			for (int k = 0; k < 4; ++k) {
				res[j*4 + i] += a[k*4 + i] * b[j*4 + k]; 
			}
		}
	}
	memcpy(a, res, 16 * sizeof(float));
}