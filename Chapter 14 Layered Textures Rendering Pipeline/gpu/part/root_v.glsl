#version 150 core

//(born moment, -life ID)
in	vec2 at_sys;
out	vec2 to_sys;

uniform vec4 cur_time;

float reset();
float update();
bool born_ready();


vec4 part_time()    {
	// frame time, life time, global time, -life ID
	return vec4(cur_time.x, cur_time.y - to_sys.x, cur_time.y, to_sys.y);
}

void main()    {
	to_sys = at_sys;
	float live = 1.0;
	if(at_sys.x > 0.0)
		live = update();
	else if( born_ready() )    {
		to_sys.x = cur_time.y;
		to_sys.y -= 1.0;
		live = reset();
	}
	to_sys.x *= 2.0*live-1.0;
}
