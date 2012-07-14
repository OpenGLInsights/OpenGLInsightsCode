#version 150 core

//#define USE_FILTER
uniform sampler2D unit_input;
const vec3 lumask = vec3(0.2125, 0.7154, 0.0721);
const float bright_add = 0.25;

noperspective in vec2 tex_coord;
out vec4 rez_color;


void main()	{
	#ifdef	USE_FILTER
	const ivec2 add = ivec2(1,0);
	vec3 color = 0.25*(
		textureOffset(unit_input, tex_coord, +add.xy)+
		textureOffset(unit_input, tex_coord, -add.xy)+
		textureOffset(unit_input, tex_coord, +add.yx)+
		textureOffset(unit_input, tex_coord, -add.yx)
	).xyz;
	#else	//USE_FILTER
	vec3 color = texture(unit_input,tex_coord).xyz;
	#endif	//USE_FILTER
	float bright = log(dot(color, lumask) + bright_add);
	color += 0.2*sin(-10.0*color/(color+vec3(2.0)));
	rez_color = vec4(color, bright);
}
