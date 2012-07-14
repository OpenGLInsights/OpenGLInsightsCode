/** ----------------------------------------------------------
 * \class VSMathLib
 *
 * Lighthouse3D
 *
 * VSMathLib - Very Simple Matrix Library
 *
 * Full documentation at 
 * http://www.lighthouse3d.com/very-simple-libs
 *
 * This class aims at easing geometric transforms, camera
 * placement and projection definition for programmers
 * working with OpenGL core versions.
 *
 * \version 0.2.1
 *		Split MODELVIEW into MODEL and VIEW
 *			VIEW_MODEL is now a computed matrix
 *		Removed ALWAYS_SEND_TO_GL
 *
 * version 0.2.0
 *		Added derived matrices
 *			Projection View Model
 *			Normal
 *		Added vector operations
 *		Library is now called vsMathLib
 *
 * version 0.1.1
 *		Added multiplication of a matrix by a point
 *		Added AUX as a matrix type
 * 
 * version 0.1.0
 *		Initial Release
 *
 * This lib requires:
 * External Dependencies
 *	GLEW (http://glew.sourceforge.net/)
 * VSL Dependencies
 *	VSShaderLib (http://www.lighthouse3d.com/very-simple-libs)
 *
 ---------------------------------------------------------------*/
#ifndef __VSMathLib__
#define __VSMathLib__

#include <vector>
#include <string>
#include <GL/glew.h>


class VSMathLib {

	public:
		/// number of settable matrices
		#define COUNT_MATRICES 4
		/// number of derived matrices
		#define COUNT_COMPUTED_MATRICES 3


		/// Enumeration of the matrix types
		enum MatrixTypes{ 
				MODEL,
				VIEW, 
				PROJECTION,
				AUX0,
				AUX1,
				AUX2,
				AUX3
		} ; 
		/// Enumeration of derived matrices
		enum ComputedMatrixTypes {
			VIEW_MODEL,
			PROJ_VIEW_MODEL,
			NORMAL
		};

		/// Singleton pattern
		static VSMathLib* gInstance;

		/// Call this to get the single instance of VSMathLib
		static VSMathLib* getInstance (void);

		~VSMathLib();


		/** Call this function to init the library if using uniform blocks
		  * Uniform blocks are considered to be shaded amongst shaders
		  *
		  * \param blockName the name of the block
		*/
		void setUniformBlockName(std::string blockName);

		/** Call this function to init the library 
		  * it associates the matrices with named uniforms
		  *
		  * \param matType the type of the matrix
		  * \param uniformName the name of the uniform variable 
		*/
		void setUniformName(MatrixTypes matType, std::string uniformName);

		/** Call this function to init the library 
		  * it associates the matrices with named uniforms in
		  * the case where they are stored in an array
		  *
		  * \param matType the type of the matrix
		  * \param uniformName the name of the uniform variable 
		  * \param index the index of the array where the mat is located
		*/
		void setUniformArrayIndexName(MatrixTypes matType, 
							std::string uniformName, int index);

		/** Call this function to init the library 
		  * it associates the matrices with named uniforms
		  *
		  * \param matType the type of the matrix
		  * \param uniformName the name of the uniform variable 
		*/
		void setUniformName(ComputedMatrixTypes matType, 
							std::string uniformName);

		/** Call this function to init the library 
		  * it associates the matrices with named uniforms in
		  * the case where they are stored in an array
		  *
		  * \param matType the type of the matrix
		  * \param uniformName the name of the uniform variable 
		  * \param index the index of the array where the mat is located
		*/
		void setUniformArrayIndexName(ComputedMatrixTypes matType, 
							std::string uniformName, int index);

		/** Similar to glTranslate*. 
		  *
		  * \param aType any value from MatrixTypes
		  * \param x,y,z vector to perform the translation
		*/
		void translate(MatrixTypes aType, float x, float y, float z);

		/** Similar to glTranslate*. Applied to MODELVIEW only.
		  *
		  * \param x,y,z vector to perform the translation
		*/
		void translate(float x, float y, float z);

		/** Similar to glScale*.
		  *
		  * \param aType any value from MatrixTypes
		  * \param x,y,z scale factors
		*/
		void scale(MatrixTypes aType, float x, float y, float z);

		/** Similar to glScale*. Applied to MODELVIEW only.
		  *
		  * \param x,y,z scale factors
		*/
		void scale(float x, float y, float z);

		/** Similar to glTotate*. 
		  *
		  * \param aType any value from MatrixTypes
		  * \param angle rotation angle in degrees
		  * \param x,y,z rotation axis in degrees
		*/
		void rotate(MatrixTypes aType, float angle, float x, float y, float z);

		/** Similar to glRotate*. Applied to MODELVIEW only.
		  *
		  * \param angle rotation angle in degrees
		  * \param x,y,z rotation axis in degrees
		*/
		void rotate(float angle, float x, float y, float z);

		/** Similar to glLoadIdentity.
		  *
		  * \param aType any value from MatrixTypes
		*/
		void loadIdentity(MatrixTypes aType);

		/** Similar to glMultMatrix.
		  *
		  * \param aType any value from MatrixTypes
		  * \param aMatrix matrix in column major order data, float[16]
		*/
		void multMatrix(MatrixTypes aType, float *aMatrix);

		/** Similar to gLoadMatrix.
		  *
		  * \param aType any value from MatrixTypes
		  * \param aMatrix matrix in column major order data, float[16]
		*/

		void loadMatrix(MatrixTypes aType, float *aMatrix);

		/** Similar to glPushMatrix
		  * 
		  * \param aType any value from MatrixTypes
		*/
		void pushMatrix(MatrixTypes aType);

		/** Similar to glPopMatrix
		  * 
		  * \param aType any value from MatrixTypes
		*/
		void popMatrix(MatrixTypes aType);

		/** Similar to gluLookAt
		  *
		  * \param xPos, yPos, zPos camera position
		  * \param xLook, yLook, zLook point to aim the camera at
		  * \param xUp, yUp, zUp camera's up vector
		*/
		void lookAt(float xPos, float yPos, float zPos,
					float xLook, float yLook, float zLook,
					float xUp, float yUp, float zUp);


		/** Similar to gluPerspective
		  *
		  * \param fov vertical field of view
		  * \param ratio aspect ratio of the viewport or window
		  * \param nearp,farp distance to the near and far planes
		*/
		void perspective(float fov, float ratio, float nearp, float farp);

		/** Similar to glOrtho and gluOrtho2D (just leave the 
		  * last two params blank).
		  *
		  * \param left,right coordinates for the left and right vertical 
		  * clipping planes
		  * \param bottom,top coordinates for the bottom and top horizontal 
		  * clipping planes
		  * \param nearp,farp distance to the near and far planes
		*/
		void ortho(float left, float right, float bottom, float top, 
						float nearp=-1.0f, float farp=1.0f);

		/** Similar to glFrustum
		  *
		  * \param left,right coordinates for the left and right vertical 
		  * clipping planes
		  * \param bottom,top coordinates for the bottom and top horizontal 
		  * clipping planes
		  * \param nearp,farp distance to the near and far planes
		*/
		void frustum(float left, float right, float bottom, float top, 
						float nearp, float farp);

		/** Similar to glGet
		  *
		  * \param aType any value from MatrixTypes
		  * \returns pointer to the matrix (float[16])
		*/
		float *get(MatrixTypes aType);

		/** Similar to glGet for computed matrices
		  *
		  * \param aType any value from ComputedMatrixTypes
		  * \returns pointer to the matrix (float[16])
		*/
		float *get(ComputedMatrixTypes aType);

		/** Updates the uniform buffer data 
		  *
		  * \param aType any value from MatrixTypes
		*/
		//void matrixToBuffer(MatrixTypes aType);

		///** Updates the uniform variables 
		//  *
		//  * \param aType any value from MatrixTypes
		//*/
		//void matrixToUniform(MatrixTypes aType);

		/** Updates either the buffer or the uniform variables 
		  * based on if the block name has been set
		  *
		  * \param aType any value from MatrixTypes
		*/
		void matrixToGL(MatrixTypes aType);	

		/** Updates either the buffer or the uniform variables 
		  * based on if the block name has been set
		  *
		  * \param aType any value from ComputedMatrixTypes
		*/
		void matrixToGL(ComputedMatrixTypes aType);	


		/** Updates either the buffer or the uniform variables 
		  * based on if the block name has been set. It updates 
		  * all matrices whose uniform names have been provided
		*/
		void matricesToGL();


		/** Computes the multiplication of a matrix and a point 
		  *
		  * \param aType any value from MatrixTypes
		  * \param point a float[4] representing a point
		  * \param res a float[4] res = M * point
		*/
		void multMatrixPoint(MatrixTypes aType, float *point, float *res);

		/** Computes the multiplication of a computed matrix and a point 
		  *
		  * \param aType any value from ComputedMatrixTypes
		  * \param point a float[4] representing a point
		  * \param res a float[4] res = M * point
		*/
		void multMatrixPoint(ComputedMatrixTypes aType, float *point, float *res);

		/** vector cross product res = a x b
		  * Note: memory for the result must be allocatted by the caller
		  * 
		  * \param a,b the two input float[3]
		  * \param res the ouput result, a float[3]
		*/
		static void crossProduct( float *a, float *b, float *res);

		/** vector dot product 
		  * 
		  * \param a,b the two input float[3]
		  * \returns the dot product a.b
		*/
		static float dotProduct(float *a, float * b);

		/// normalize a vec3
		static void normalize(float *a);

		/// vector subtraction res = a - b
		static void subtract( float *a, float *b, float *res);

		/// vector addition res = a + b
		static void add( float *a, float *b, float *res);

		/// vector length
		static float length(float *a);

	protected:

		VSMathLib();

		/// aux variable to hold the result of vector ops
		float mPointRes[4];

		/// Has an init* function been called?
		bool mInit;

		/// Using uniform blocks?
		bool mBlocks;

		/// Matrix stacks for all matrix types
		std::vector<float *> mMatrixStack[COUNT_MATRICES];

		/// The storage for matrices
		float mMatrix[COUNT_MATRICES][16];
		float mCompMatrix[COUNT_COMPUTED_MATRICES][16];

		/// Stores the uniform block name
		std::string mBlockName;
		
		/// Names of the associated uniform variables
		std::string mUniformName[COUNT_MATRICES];
		int mUniformArrayIndex[COUNT_MATRICES];

		/// Names of the associated uniform variables
		std::string mComputedMatUniformName[COUNT_COMPUTED_MATRICES];
		int mComputedMatUniformArrayIndex[COUNT_COMPUTED_MATRICES];

		/// The projection matrix
	//	float mProjModelView[16];
		/// The ViewModel matrix
	//	float mViewModel[16];

		/// The normal matrix
		float mNormal[12];
		float mNormal3x3[9];
		
		/// aux 3x3 matrix
		float mMat3x3[9];

		// AUX FUNCTIONS

		/** Set a float* to an identity matrix
		  *
		  * \param a float array with the matrix contents
		  * \param size the order of the matrix
		*/
		void setIdentityMatrix( float *mat, int size=4);

		/// Computes the normal matrix based on the modelview matrix
		void computeNormalMatrix();
		/// Computes the normal matrix for use with glUniform
		void computeNormalMatrix3x3();

		/// Computes Derived Matrices (4x4)
		void computeDerivedMatrix(ComputedMatrixTypes aType);

		//resMatrix = resMatrix * aMatrix
		void multMatrix(float *resMatrix, float *aMatrix);


};

#endif