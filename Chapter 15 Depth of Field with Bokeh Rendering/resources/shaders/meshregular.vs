#version 420 core

#ifdef GBUFFER
	uniform mat4 Transform;
	uniform mat4 Model;

	layout(location = ATTR_POSITION) 	in  vec3 Position;
	layout(location = ATTR_NORMAL) 		in  vec3 Normal;
	layout(location = ATTR_TEXCOORD) 	in  vec2 TexCoord;
	layout(location = ATTR_TANGENT) 	in  vec4 Tangent;

	out vec3  vPosition;
	out vec3  vNormal;
	out vec3  vTangent;
	out vec2  vTexCoord;
	out float vTBNsign;

	void main()
	{
		// Do not support non uniform scale
		mat3 model3x3= mat3(Model);
		gl_Position  = Transform * Model * vec4(Position,1.f);
		vPosition	 = (Model * vec4(Position,1.f)).xyz;
		vNormal	 	 = model3x3 * Normal;
		vTangent 	 = model3x3 * Tangent.xyz;
		vTBNsign	 = Tangent.w;
		vTexCoord 	 = TexCoord;
	}
#endif


#ifdef CSM_BUILDER
	uniform mat4 View;
	uniform mat4 Model;
	layout(location = ATTR_POSITION) in  vec3 Position;

	void main()
	{
		gl_Position  = View * Model * vec4(Position,1.f);
	}
#endif
