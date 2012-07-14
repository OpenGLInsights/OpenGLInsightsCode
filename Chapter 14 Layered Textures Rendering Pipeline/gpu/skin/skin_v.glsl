#version 150 core

in	uvec4 at_skin;

const int NB = 100;
uniform struct Spatial	{
	vec4 pos,rot;
}bone[NB];

//a special uniform, allowing the program to ask this shader
//for a maximum supported bones number
uniform	int	bones_number	= NB;


void skin_append(float,Spatial);
Spatial skin_result();


Spatial skin_append_all()	{
	uvec4 ids = (at_skin + uint(bones_number>>20)) >> 8u;
	vec4 wes = vec4(at_skin & uvec4(255)) * (1.0/255.0);
	for(int i=0; i<4; ++i)
		skin_append( wes[i], bone[int(ids[i])] );
	return skin_result();
}
