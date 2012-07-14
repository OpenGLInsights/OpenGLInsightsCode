//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <glf/io/scene.hpp>
#include <glf/io/model.hpp>
#include <glf/io/config.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

namespace glf
{
	namespace io
	{
		//----------------------------------------------------------------------
		void LoadScene(		const std::string& _filename,
							ResourceManager& _resourceManager,
							SceneManager& _scene,
							bool _verbose)
		{
			// Load configuration file
			glf::io::ConfigLoader loader;
			glf::io::ConfigNode* root	= loader.Load(_filename);

			// Load models
			glf::io::ConfigNode* geometriesNode = loader.GetNode(root,"geometries");
			if(geometriesNode != NULL)
			{
				int nGeometries = loader.GetCount(geometriesNode);
				for(int i=0;i<nGeometries;++i)
				{
					glf::io::ConfigNode* geometryNode = loader.GetNode(geometriesNode,i);

					std::string name			= loader.GetString(geometryNode,"name");
					std::string folder			= loader.GetString(geometryNode,"folder");
					std::string filename		= loader.GetString(geometryNode,"file");
					glm::vec3 translate			= loader.GetVec3(geometryNode,"translate");
					glm::vec3 rotate			= loader.GetVec3(geometryNode,"rotate");
					float scale					= loader.GetFloat(geometryNode,"scale");
					glm::mat4 transform			=	glm::translate(translate.x,translate.y,translate.z) *
													glm::rotate(rotate.z,0.f,0.f,1.f) *
													glm::rotate(rotate.y,0.f,1.f,0.f) *
													glm::rotate(rotate.x,1.f,0.f,0.f) *
													glm::scale(scale,scale,scale);

					LoadModel(	glf::directory::ModelDirectory + folder + "/",
								filename,
								transform,
								_resourceManager,
								_scene,
								_verbose);
				}
			}

			// Load terrains
			glf::io::ConfigNode* terrainsNode = loader.GetNode(root,"terrains");
			if(terrainsNode != NULL)
			{
				int nTerrains = loader.GetCount(terrainsNode);
				for(int i=0;i<nTerrains;++i)
				{
					glf::io::ConfigNode* terrainNode = loader.GetNode(terrainsNode,i);

					std::string name			= loader.GetString(terrainNode,"name");
					std::string diffuse			= loader.GetString(terrainNode,"diffuse");
					std::string height			= loader.GetString(terrainNode,"height");

					float roughness				= loader.GetFloat(terrainNode,"roughness");
					float specularity			= loader.GetFloat(terrainNode,"specularity");

					glm::vec2 terrainSize		= loader.GetVec2(terrainNode,"terrainSize");
					glm::vec3 terrainOffset		= loader.GetVec3(terrainNode,"terrainOffset");
					int tileResolution			= 32; //loader.GetInt(terrainNode,"tileResolution");
					float heightFactor			= loader.GetFloat(terrainNode,"heightFactor");
					float tileFactor			= loader.GetFloat(terrainNode,"tileFactor");
					float tessFactor			= 15.f; //loader.GetFloat(terrainNode,"tessFactor");
					float projFactor			= 10.f; //loader.GetFloat(terrainNode,"projFactor");

					LoadTerrain(glf::directory::TextureDirectory,
								diffuse,
								height,
								roughness,
								specularity,
								terrainSize,
								terrainOffset,
								tileResolution,
								heightFactor,
								tessFactor,
								projFactor,
								tileFactor,
								_resourceManager,
								_scene,
								_verbose);
				}
			}

			// Compute bounds
			_scene.wBound = WorldBound(_scene);
			if(_verbose)
			{
				glf::Info("----------------------------------------------");
				glf::Info("World bound   : (%f,%f,%f) (%f,%f,%f)",
											_scene.wBound.pMin.x,
											_scene.wBound.pMin.y,
											_scene.wBound.pMin.z,
											_scene.wBound.pMax.x,
											_scene.wBound.pMax.y,
											_scene.wBound.pMax.z);
			}

			// Load lights
			//TODO

			// Load camera
			//TODO
		}
		//----------------------------------------------------------------------
	}
}
