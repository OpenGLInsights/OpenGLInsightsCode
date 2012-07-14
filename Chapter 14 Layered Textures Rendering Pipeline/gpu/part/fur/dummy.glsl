#version 150 core

in vec2 at_sys;
out vec2 to_sys;

void init_dummy()	{
	to_sys = vec2(0.0);
}

float reset_dummy()	{
	to_sys = at_sys*0.00001 + vec2(2.0);
	return 1.0;
}

float update_dummy()	{
	to_sys = at_sys;
	return 1.0;
}