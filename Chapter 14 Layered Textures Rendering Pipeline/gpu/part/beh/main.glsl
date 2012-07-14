#version 150 core

//(size,life)
in	vec2 at_sub;
in	vec3 at_pos, at_speed;
out	vec2 to_sub;
out	vec3 to_pos, to_speed;


uniform struct Spatial	{
	vec4 pos,rot;
}s_model;

uniform vec4 part_speed_tan;	//w == tangental rotation
uniform vec4 part_speed_obj;	//w == random contribution
uniform vec4 object_speed;	//pre-multiplied object speed
uniform vec4 part_size,part_life;	//x +- y


vec4 part_time();
float part_uni();
float random(float);
Spatial get_surface(vec2);
vec3 qrot(vec4,vec3);

vec2 random2(vec2 seed)	{
	float uni = part_uni();
	float r = random(uni + seed.x);
	return vec2(r, random(r + seed.y));
}


void init_main()	{
	to_sub = vec2(0.0);
	to_speed = to_pos = vec3(0.0);
}

float reset_main()	{
	vec4 pt = part_time();
	vec2 rp2 = random2(pt.xy);
	float rs1 = 2.0*rp2.x-1.0;
	vec4 sub = vec4(part_size.xy, part_life.xy);
	to_sub = sub.xz * (vec2(1.0) + rs1 * sub.yw);
	Spatial surf = get_surface(rp2);
	to_pos = surf.pos.xyz;
	vec3 hand = vec3( surf.pos.w, 1.0,1.0);
	// generating random vec3 - bad approach
	vec3 dir = vec3( random2(pt.yx), random(rp2.y+rs1) );
	dir = normalize( 2*dir-vec3(1.0) );
	to_speed = object_speed.xyz + dir * part_speed_obj.w +
		qrot( surf.rot, hand *	part_speed_tan.xyz )  +
		qrot( s_model.rot,	part_speed_obj.xyz );
	return step(0.01, dot(to_pos,to_pos) );
}

float update_main()	{
	vec4 pt = part_time();
	to_sub = at_sub;
	to_pos = at_pos + pt.x * at_speed;
	to_speed = at_speed;
	return step( pt.y, at_sub.y );
}
