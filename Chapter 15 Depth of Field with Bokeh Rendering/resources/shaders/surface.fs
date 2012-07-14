#version 330

#ifdef REGULAR
uniform sampler2D   Texture;
uniform float       Level;
out vec4            FragColor;

void main()
{
	FragColor = textureLod(Texture, gl_FragCoord.xy * vec2(RCP_SCREEN_X,RCP_SCREEN_Y), Level);
}
#endif

#ifdef ARRAY
uniform sampler2DArray 	Texture;
uniform float       	Level;
uniform float       	Layer;
out vec4            	FragColor;

void main()
{
	FragColor = textureLod(Texture, vec3(gl_FragCoord.xy * vec2(RCP_SCREEN_X,RCP_SCREEN_Y),Layer), Level);
}
#endif
