#version 150 core

in vec4 to_old,to_new;
out vec2 to_speed;

void main()	{
	to_speed = to_new.xy/to_new.w - to_old.xy/to_old.w;
}