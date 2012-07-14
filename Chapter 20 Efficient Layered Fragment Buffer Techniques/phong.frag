/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

#version 420

in vec3 osFrag;
in vec3 esFrag;
in vec3 esLight;
in vec3 esNorm;
in vec2 coord;

out vec4 fragColour;

uniform float alpha;

layout(rgba32f) uniform imageBuffer test;

uniform sampler2D texColour;
uniform int textured;

vec3 shadeStrips()
{
	vec3 col;
	float i = floor(osFrag.x * 32.0f);
	col.rgb = (fract(i*0.5f) == 0.0f) ? vec3(0.37f, 0.81f, 0.0f) : vec3(1.0f);
	return col;
}

void main()
{
	vec3 L = normalize(esLight);
	vec3 N = normalize(esNorm);
	vec3 E = normalize(-esFrag);
	
	if (textured == 0)
		fragColour = vec4(shadeStrips(), 1.0);
	else
	{
		fragColour = texture(texColour, coord);
		if (fragColour.a < 0.5)
			discard;
	}
	fragColour.a *= alpha;
	
	float diffuse = abs(dot(E, N));
	if (diffuse > 0.0)
	{
		fragColour.rgb *= diffuse;
		//vec3 R = reflect(-L, N);
		//float specular = pow(max(0.0, dot(R, E)), 50.0);
		//fragColour.rgb += vec3(specular * 0.3);
	}
}
