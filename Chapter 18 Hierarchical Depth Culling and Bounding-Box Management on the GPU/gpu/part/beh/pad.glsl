#version 150 core

in	float at_pad;
out	float to_pad;

void init_pad()	{
	to_pad = 0.0;
}

float reset_pad()	{
	to_pad = 1.0;
	return 1.0;
}

float update_pad()	{
	to_pad = 2.0;
	return 1.0;
}