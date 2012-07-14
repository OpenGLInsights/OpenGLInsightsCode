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
 * This lib provides creates a terrain model based on a height map 
 *
 * Requirements: Devil, and tinyXML
 *
 * Full documentation at 
 * http://www.lighthouse3d.com/very-simple-libs
 *
 ---------------------------------------------------------------*/



#ifndef __VSTerrainLib__
#define __VSTerrainLib__

#include <string>
#include <vector>
#include <map>
#include <fstream>

#include <GL/glew.h>

// include Resource Lib, from which it derives
#include "vsResourceLib.h"


class VSTerrainLib: public VSResourceLib{

public:

	VSTerrainLib();
	~VSTerrainLib();

	virtual bool load(std::string filename);
	virtual void render();

	virtual void addTexture(unsigned int unit, std::string filename);

	void setHeightStep(float f);
	void setGridSpacing(float x);
	float getGridSpacing();
	virtual float getHeight( float x, float z);
	int getGridSize();

	void setTextureCoordinates(int mode);

protected:
	void setNormals();

	virtual float getHeight(int x, int z);
	int mScaleFactor;
	float mGridSpacing;
	unsigned int mGridSizeX, mGridSizeZ;
	float mHeightStep;
	unsigned short *mHeights;

	struct MyMesh{

		GLuint vao;
		GLuint texUnits[MAX_TEXTURES];
		GLuint uniformBlockIndex;
		float transform[16];
		int numIndices;
		unsigned int type;
		struct Material mat;
	};

	struct MyMesh mMesh;
};

#endif
