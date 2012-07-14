#version 420

vec3 array[8];
vec4 frags[8];

void swap(int i, int j)
{
	if (array[i].x > array[j].x || (array[i].x == array[j].x && array[i].y < array[j].y))
	{
		vec3 t = array[i];
		array[i] = array[j];
		array[j] = t;
	}
}

layout(size1x32) uniform uimageBuffer fragIDs;
layout(size4x32) uniform imageBuffer data;

void main()
{
	int i = gl_VertexID * 8;
	
	//load data and create sorting array
	for (int j = 0; j < 8; ++j)
	{
		uint id = imageLoad(fragIDs, i + j).r;
		array[j].x = float(id) + 0.5;
		frags[j] = imageLoad(data, i + j);
		array[j].y = frags[j].a;
		array[j].z = j;
	}

	//http://pages.ripco.net/~jgamble/nw.html
	//Network for N=8, using Bose-Nelson Algorithm.
	swap(0, 1);
	swap(2, 3);
	swap(0, 2);
	swap(1, 3);
	swap(1, 2);
	swap(4, 5);
	swap(6, 7);
	swap(4, 6);
	swap(5, 7);
	swap(5, 6);
	swap(0, 4);
	swap(1, 5);
	swap(1, 4);
	swap(2, 6);
	swap(3, 7);
	swap(3, 6);
	swap(2, 4);
	swap(3, 5);
	swap(3, 4);

	//save data ordered by sorting array	
	for (int j = 0; j < 8; ++j)
	{
		imageStore(data, i + j, frags[int(array[j].z)]);
	}
}
