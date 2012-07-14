//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <glf/wrapper.hpp>
#include <glf/debug.hpp>
#include <sstream>

namespace glf
{
	//-------------------------------------------------------------------------
	void ProgramOptions::AddResolution(	const std::string& _name,  
										int _resX, 
										int _resY)
	{
		std::stringstream option;
		option << "#define " << _name << "_X " << _resX << std::endl;
		option << "#define " << _name << "_Y " << _resY << std::endl;
		option << "#define RCP_" << _name << "_X " << 1.f/float(_resX) << std::endl;
		option << "#define RCP_" << _name << "_Y " << 1.f/float(_resY) << std::endl;
		options.push_back(option.str());
	}
	//-------------------------------------------------------------------------
	void ProgramOptions::AddConst(	const std::string& _name,  
									int _value)
	{
		std::stringstream option;
		option << "const int " << _name << " = " << _value << ";";
		options.push_back(option.str());
	}
	//-------------------------------------------------------------------------
	void ProgramOptions::AddConst(	const std::string& _name,  
									float _value)
	{
		std::stringstream option;
		option << "const float " << _name << " = " << _value << ";";
		options.push_back(option.str());
	}
	//-------------------------------------------------------------------------
	void ProgramOptions::AddConst(	const std::string& _name,  
									const glm::ivec2& _value)
	{
		std::stringstream option;
		option << "const ivec2 " << _name << " = ivec2(" << _value.x << "," << _value.y << ");";
		options.push_back(option.str());
	}
	//-------------------------------------------------------------------------
	void ProgramOptions::Include(	const std::string& _fileContent)
	{
		options.push_back(_fileContent);
	}
	//-------------------------------------------------------------------------
	std::string ProgramOptions::ToString(	) const
	{
		std::stringstream out;
		for(unsigned int i=0;i<options.size();++i)
			out << options[i] << std::endl;
		return out.str();
	}
	//-------------------------------------------------------------------------
	std::string ProgramOptions::Append(	const std::string& _input) const
	{
		std::vector<std::string> lines;
		Split(_input, '\n',lines);
	
		// Look for #version
		std::string version		= "#version";
		unsigned int lineIndex	= 0;
		bool versionFound		= false;
		while(lineIndex<lines.size() && !versionFound)
		{
			versionFound = lines[lineIndex].compare(0,8,version,0,8) == 0;
			if(!versionFound) ++lineIndex;
		}
		assert(versionFound);

		// Merge output lines		
		std::stringstream output;
		for(unsigned int i=0;i<=lineIndex;++i)
			output << lines[i] << std::endl;
		output << ToString();
		for(unsigned int i=lineIndex+1;i<lines.size();++i)
			output << lines[i] << std::endl;

		return output.str();
	}
	//-------------------------------------------------------------------------
	ProgramOptions ProgramOptions::CreateVSOptions()
	{
		ProgramOptions options;
		options.AddDefine<int>("ATTR_POSITION",	semantic::Position);
		options.AddDefine<int>("ATTR_NORMAL",	semantic::Normal);
		options.AddDefine<int>("ATTR_TEXCOORD",	semantic::TexCoord);
		options.AddDefine<int>("ATTR_TANGENT",	semantic::Tangent);
		options.AddDefine<int>("ATTR_COLOR",	semantic::Color);
		options.AddDefine<int>("ATTR_BITANGENT",semantic::Bitangent);
		return options;
	}
	//-------------------------------------------------------------------------
	ProgramOptions ProgramOptions::CreateFSOptions(int _w, int _h)
	{
		ProgramOptions options;
		options.AddResolution("SCREEN_RESOLUTION",_w,_h);
		return options;
	}
	//-------------------------------------------------------------------------
	Variable::Variable():
	name(""),
	category(MAX),
	type(-1),
	location(-1),
	unit(-1)
	{

	}
	//-------------------------------------------------------------------------
	std::string Variable::ToString() const
	{
		std::stringstream out;
		out << std::endl;
		out << "Name     : " << name     << std::endl;
		out << "Type     : " << type     << std::endl;
		out << "Location : " << location << std::endl;
		out << "Unit     : " << unit     << std::endl;
		out << "Category : " << category << std::endl;
		return out.str();
	}
	//-------------------------------------------------------------------------
	Program::Program(const std::string& _name):
	id(-1),
	name(_name),
	compiled(false)
	{

	}
	//-------------------------------------------------------------------------
	Program::~Program()
	{
		if(id!=1)
			glDeleteProgram(id);
	}
	//-------------------------------------------------------------------------
	bool Program::IsTextureSampler(GLenum _type)
	{
		return  _type == GL_SAMPLER_1D ||
				_type == GL_SAMPLER_2D ||
				_type == GL_SAMPLER_3D ||
				_type == GL_SAMPLER_CUBE ||
				_type == GL_SAMPLER_1D_SHADOW ||
				_type == GL_SAMPLER_2D_SHADOW ||
				_type == GL_SAMPLER_1D_ARRAY ||
				_type == GL_SAMPLER_2D_ARRAY ||
				_type == GL_SAMPLER_CUBE_MAP_ARRAY ||
				_type == GL_SAMPLER_1D_ARRAY_SHADOW ||
				_type == GL_SAMPLER_2D_ARRAY_SHADOW ||
				_type == GL_SAMPLER_2D_MULTISAMPLE ||
				_type == GL_SAMPLER_2D_MULTISAMPLE_ARRAY ||
				_type == GL_SAMPLER_CUBE_SHADOW ||
				_type == GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW ||
				_type == GL_SAMPLER_BUFFER ||
				_type == GL_SAMPLER_2D_RECT ||
				_type == GL_SAMPLER_2D_RECT_SHADOW ||
				_type == GL_INT_SAMPLER_1D ||
				_type == GL_INT_SAMPLER_2D ||
				_type == GL_INT_SAMPLER_3D ||
				_type == GL_INT_SAMPLER_CUBE ||
				_type == GL_INT_SAMPLER_1D_ARRAY ||
				_type == GL_INT_SAMPLER_2D_ARRAY ||
				_type == GL_INT_SAMPLER_CUBE_MAP_ARRAY ||
				_type == GL_INT_SAMPLER_2D_MULTISAMPLE ||
				_type == GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY ||
				_type == GL_INT_SAMPLER_BUFFER ||
				_type == GL_INT_SAMPLER_2D_RECT ||
				_type == GL_UNSIGNED_INT_SAMPLER_1D ||
				_type == GL_UNSIGNED_INT_SAMPLER_2D ||
				_type == GL_UNSIGNED_INT_SAMPLER_3D ||
				_type == GL_UNSIGNED_INT_SAMPLER_CUBE ||
				_type == GL_UNSIGNED_INT_SAMPLER_1D_ARRAY ||
				_type == GL_UNSIGNED_INT_SAMPLER_2D_ARRAY ||
				_type == GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY ||
				_type == GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE ||
				_type == GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY ||
				_type == GL_UNSIGNED_INT_SAMPLER_BUFFER ||
				_type == GL_UNSIGNED_INT_SAMPLER_2D_RECT ||
				_type == GL_IMAGE_1D ||
				_type == GL_IMAGE_2D ||
				_type == GL_IMAGE_3D ||
				_type == GL_IMAGE_2D_RECT ||
				_type == GL_IMAGE_CUBE ||
				_type == GL_IMAGE_BUFFER ||
				_type == GL_IMAGE_1D_ARRAY ||
				_type == GL_IMAGE_2D_ARRAY ||
				_type == GL_IMAGE_CUBE_MAP_ARRAY ||
				_type == GL_IMAGE_2D_MULTISAMPLE ||
				_type == GL_IMAGE_2D_MULTISAMPLE_ARRAY ||
				_type == GL_INT_IMAGE_1D ||
				_type == GL_INT_IMAGE_2D ||
				_type == GL_INT_IMAGE_3D ||
				_type == GL_INT_IMAGE_2D_RECT ||
				_type == GL_INT_IMAGE_CUBE ||
				_type == GL_INT_IMAGE_BUFFER ||
				_type == GL_INT_IMAGE_1D_ARRAY ||
				_type == GL_INT_IMAGE_2D_ARRAY ||
				_type == GL_INT_IMAGE_CUBE_MAP_ARRAY ||
				_type == GL_INT_IMAGE_2D_MULTISAMPLE ||
				_type == GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY ||
				_type == GL_UNSIGNED_INT_IMAGE_1D ||
				_type == GL_UNSIGNED_INT_IMAGE_2D ||
				_type == GL_UNSIGNED_INT_IMAGE_3D ||
				_type == GL_UNSIGNED_INT_IMAGE_2D_RECT ||
				_type == GL_UNSIGNED_INT_IMAGE_CUBE ||
				_type == GL_UNSIGNED_INT_IMAGE_BUFFER ||
				_type == GL_UNSIGNED_INT_IMAGE_1D_ARRAY ||
				_type == GL_UNSIGNED_INT_IMAGE_2D_ARRAY ||
				_type == GL_UNSIGNED_INT_IMAGE_CUBE_MAP_ARRAY ||
				_type == GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE ||
				_type == GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY;
	}
	//-------------------------------------------------------------------------
	void Program::AnalyzeProgram(const std::string& _name, GLuint _id, std::map<std::string,Variable>& _variables)
	{
		// Get number of elements
		int nAttributes, nUniforms, nBlocks;
		glGetProgramiv(_id, GL_ACTIVE_ATTRIBUTES, 		&nAttributes);
		glGetProgramiv(_id, GL_ACTIVE_UNIFORMS, 			&nUniforms);
		glGetProgramiv(_id, GL_ACTIVE_UNIFORM_BLOCKS, 	&nBlocks);
		
		#if ENABLE_VERBOSE_PROGRAM
		Info("---------------------------------------------------------------");
		Info("Program : %s",_name.c_str());
		#endif

		// Get "ATTRIBUTS"
		const int ShaderAttributeMaxLength = 100;
		GLsizei maxLength, length;
		GLint size;
		GLenum type;
		GLchar name[ShaderAttributeMaxLength];
		maxLength = ShaderAttributeMaxLength;
		#if ENABLE_VERBOSE_PROGRAM
		Info("nAttributes : %d",nAttributes);
		#endif
		for(int index=0; index<nAttributes; ++index)
		{
			glGetActiveAttrib(_id, index, maxLength, &length, &size, &type, name);
			Variable var;
			var.name 	 = name;
			var.location = glGetAttribLocation(_id, name);
			var.type 	 = type;
			var.category = Variable::ATTRIBUTE;

			_variables[name] = var;
			#if VERBOSE_PROGRAM
			Info(var.ToString());
			#endif
		}

		// Get "UNIFORMS"
		int textureUnit = 0;
		#if ENABLE_VERBOSE_PROGRAM
		Info("nUniforms : %d",nUniforms);
		#endif
		for(int index=0; index<nUniforms; ++index) 
		{
			// Get variables
			glGetActiveUniform(_id, index, maxLength, &length, &size, &type, name);
			Variable var;
			var.name 	 = name;
			var.location = glGetUniformLocation(_id, name);
			var.type 	 = type;
			if(IsTextureSampler(type))
				var.unit = textureUnit++;
			var.category = Variable::UNIFORM;

			_variables[name] = var;
			#if VERBOSE_PROGRAM
			Info(var.ToString());
			#endif
		}

		// Get "BLOCKS"
		#if ENABLE_VERBOSE_PROGRAM
		Info("nBlocks : %d",nBlocks);
		#endif
		for(int index=0; index<nBlocks; ++index) 
		{
		 	glGetActiveUniformBlockName(_id, index, maxLength, &length, name);
			Variable var;
			var.name 	 = name;
			var.location = glGetUniformBlockIndex(_id, name);
			var.type 	 = type;
			var.unit	 = var.location;
			var.category = Variable::BLOCK;

			_variables[name] = var;
			#if VERBOSE_PROGRAM
			Info(var.ToString());
			#endif
		}

		assert(CheckError("Program::AnalyzeProgram"));
	}
	//-------------------------------------------------------------------------
	bool Program::Compile(			const std::string& _vFile,
									const std::string& _fFile)
	{
		id = CreateProgram(name,_vFile,"","","",_fFile);
		AnalyzeProgram(name,id,variables);
		return true;
	}
	//-------------------------------------------------------------------------
	bool Program::Compile(			const std::string& _vFile,
									const std::string& _gFile,
									const std::string& _fFile)
	{
		id = CreateProgram(name,_vFile,"","",_gFile,_fFile);
		AnalyzeProgram(name,id,variables);
		return true;
	}
	//-------------------------------------------------------------------------
	bool Program::Compile(			const std::string& _vFile,
									const std::string& _cFile,
									const std::string& _eFile,
									const std::string& _fFile)
	{
		id = CreateProgram(name,_vFile,_cFile,_eFile,"",_fFile);
		AnalyzeProgram(name,id,variables);
		return true;
	}
	//-------------------------------------------------------------------------
	bool Program::Compile(			const std::string& _vFile,
									const std::string& _cFile,
									const std::string& _eFile,
									const std::string& _gFile,
									const std::string& _fFile)
	{
		id = CreateProgram(name,_vFile,_cFile,_eFile,_gFile,_fFile);
		AnalyzeProgram(name,id,variables);
		return true;
	}
	//-------------------------------------------------------------------------
	const Variable& Program::operator[](const std::string& _varName) const
	{
		std::map<std::string,Variable>::const_iterator it;
		it = variables.find(_varName);
		if(it==variables.end())
		{
			glf::Error("No variable '%s'",_varName.c_str());
			assert(false);
		}
		return it->second;
	}
	//-------------------------------------------------------------------------
	GLint 	Program::Output(const std::string& _outName) const
	{
		GLint index = glGetFragDataLocation(id,_outName.c_str());
		if(index==INVALID_ID)
		{
			glf::Error("Variable '%s' is not a valid output variable",_outName.c_str());
			assert(false);
		}
		return index;
	}
	//-------------------------------------------------------------------------
	std::string Program::ToString() const
	{
		std::stringstream out;
		out << "Shader : " << name << std::endl << std::endl;
		std::map<std::string,Variable>::const_iterator it;
		for(it=variables.begin();it!=variables.end();++it)
			out << it->first.c_str() << " : " << it->second.ToString().c_str();

		return out.str();
	}
	//-------------------------------------------------------------------------
}

