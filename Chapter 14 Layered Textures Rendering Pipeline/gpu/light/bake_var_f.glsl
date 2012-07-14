#version 150 core

in float depth;
out vec2 rez_color;

void main()	{
	rez_color = vec2(depth, depth*depth);
}
