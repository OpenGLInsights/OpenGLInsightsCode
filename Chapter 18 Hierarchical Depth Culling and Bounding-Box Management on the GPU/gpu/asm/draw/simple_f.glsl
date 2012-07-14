#version 150 core

in	vec3	v_lit;
out	vec4	to_color;

void main()	{
	float prod = max( 0.0, normalize(v_lit).z );
	to_color = vec4(prod);
}