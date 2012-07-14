#ifndef GLF_IO_CONFIG_HPP
#define GLF_IO_CONFIG_HPP

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <string>
#include <glm/glm.hpp>

namespace glf
{
	namespace io
	{
		//----------------------------------------------------------------------
		struct ConfigNode // From cJSON, see .cpp
		{
			struct ConfigNode *next,*prev;
			struct ConfigNode *child;
			int 			type;
			char*			valuestring;
			int 			valueint;
			double 			valuedouble;
			char*			string;
		};
		//----------------------------------------------------------------------
		class ConfigLoader
		{
		public:
							ConfigLoader();
							~ConfigLoader();
			ConfigNode*		Load(		const std::string& _filename);
			ConfigNode*		GetNode(	ConfigNode* _node,
										const std::string& _tag) const;
			ConfigNode*		GetNode(	ConfigNode* _node,
										int _i) const;
			bool			GetBool(	ConfigNode* _node,
										const std::string& _tag,
										bool _default = false) const;
			int				GetInt(		ConfigNode* _node,
										const std::string& _tag,
										int _default = 0) const;
			glm::ivec2		GetIVec2(	ConfigNode* _node,
										const std::string& _tag,
										glm::ivec2 _default = glm::ivec2(0)) const;
			float			GetFloat(	ConfigNode* _node,
										const std::string& _tag,
										float _default = 0) const;
			glm::vec2		GetVec2(	ConfigNode* _node,
										const std::string& _tag,
										glm::vec2 _default = glm::vec2(0)) const;
			glm::vec3		GetVec3(	ConfigNode* _node,
										const std::string& _tag,
										glm::vec3 _default = glm::vec3(0)) const;
			glm::vec4		GetVec4(	ConfigNode* _node,
										const std::string& _tag,
										glm::vec4 _default = glm::vec4(0)) const;
			std::string		GetString(	ConfigNode* _node,
										const std::string& _tag,
										const std::string& _default = "") const;
			int				GetCount(	ConfigNode* _node) const;
		private:
			ConfigNode*		root;
		};
		//----------------------------------------------------------------------
	}
}
#endif
