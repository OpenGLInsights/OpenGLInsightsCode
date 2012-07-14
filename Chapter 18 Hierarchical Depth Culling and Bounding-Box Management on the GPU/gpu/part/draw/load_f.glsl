#version 150 core

uniform vec4 halo_data;

out vec4 rez_color;

const float dark = 2.0;

vec4 get_diffuse();

void main()	{
	vec2 rad = 2.0*gl_PointCoord - vec2(1.0);
	//float r3 = 1.0-dot(rad,rad);	//old one
	float h = dark*(0.1 + 0.01 * halo_data.y);
	//hyper-circle equasion x^n + y^n = 1
	float r2 = pow( dot(rad,rad), 0.5/h );
	float r3 = pow( 1.0-r2, h );
	rez_color = get_diffuse() * vec4(halo_data.z + r3);
	if( rez_color.w < 0.01 ) discard;
}
