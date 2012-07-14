/** ----------------------------------------------------------
 * \class VSTerrainLODLib
 *
 * Lighthouse3D
 *
 * Very Simple Terrain LOD Library
 *
 * \version 0.1.0
 * Initial Release
 *
 * This lib provides creates a terrain model based on a height map 
 * using dynamic tesselation as a LOD mechanism
 *
 * Requirements: Devil, VSML, VSLL and VSShaderLib, VSTerrainLib, VSRL
 *
 * Full documentation at 
 * http://www.lighthouse3d.com/very-simple-libs
 *
 ---------------------------------------------------------------*/



#ifndef __VSTerrainLODSingleScaledLib__
#define __VSTerrainLODSingleScaledLib__

#include <string>
#include <vector>
#include <map>
#include <fstream>

#include <GL/glew.h>

#include "vsTerrainLib.h"

class VSTerrainLODSingleScaledLib: public VSTerrainLib{

public:

	VSTerrainLODSingleScaledLib();
	~VSTerrainLODSingleScaledLib();

	virtual bool load(std::string filename);
	virtual void render();

	void setScaleFactor(int f);
	int getScaleFactor();
//	virtual float getHeight( float x, float z);

protected:

//	virtual float getHeight(int x, int z);

//	int mScaleFactor;

};

#endif
