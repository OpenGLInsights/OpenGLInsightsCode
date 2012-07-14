//-----------------------------------------------------------------------------
#version 410

uniform sampler2D ColorTex;
uniform float     Exposure;
out vec4          FragColor;

//------------------------------------------------------------------------------
// Applies the filmic curve from John Hable's presentation
// More details at : http://filmicgames.com/archives/75
vec3 ToneMapFilmicALU(vec3 _color)
{
	_color = max(vec3(0), _color - vec3(0.004f));
	_color = (_color * (6.2f*_color + vec3(0.5f))) / (_color * (6.2f * _color + vec3(1.7f)) + vec3(0.06f));
	return _color;
}
//------------------------------------------------------------------------------
void main()
{
	vec2  pix 		 = gl_FragCoord.xy/vec2(textureSize(ColorTex,0).xy);
	vec3  color		 = textureLod(ColorTex, pix, 0).xyz;
	vec3 expoColor	 = color * Exposure;
	vec3 toneColor	 = ToneMapFilmicALU(expoColor);
	FragColor		 = vec4(toneColor,1);
}
