#version 150 core

in	uint	at_index;
out	uint	to_index;
out	uint	to_mat;

uniform	int	index, offset;


void main()	{
	to_index = offset + at_index;
	to_mat = index;
}