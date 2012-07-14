#ifndef GLF_IO_MODEL_HPP
#define GLF_IO_SCENE_HPP

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <string>
#include <glf/scene.hpp>
#include <glf/helper.hpp>

namespace glf
{
	namespace io
	{
		void LoadScene(		const std::string& _filename,
							ResourceManager& _resourceManager,
							SceneManager& _scene,
							bool _verbose=false);
	}
}

#endif
