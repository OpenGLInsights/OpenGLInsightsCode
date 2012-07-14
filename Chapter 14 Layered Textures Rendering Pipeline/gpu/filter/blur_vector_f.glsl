#version 150 core

noperspective in vec2 tex_coord;

uniform sampler2D unit_input;

const float offset[3] = float[]( 0.0, 1.3846153846, 3.2307692308 );
const float weight[3] = float[]( 0.2270270270, 0.3162162162, 0.0702702703 );


vec2 blur_vector();

vec4 blur_result()	{
	vec2 vl = blur_vector();
	vec4 rez = vec4(0.0);
	for(int i=0; i<3; ++i)	{
		vec2 tc = tex_coord - offset[i] * vl;
		rez += weight[i] * texture(unit_input,tc);
	}
	return rez;
}
