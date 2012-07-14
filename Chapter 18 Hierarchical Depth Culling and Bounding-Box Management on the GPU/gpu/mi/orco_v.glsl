#version 150 core

uniform	struct BBox	{
	vec4	center, hsize;
} bb_model;

in vec4 at_vertex;

vec3 mi_orco()	{
	vec3 normalized = ((at_vertex - bb_model.center) / bb_model.hsize).xyz;
	return 0.5 * (normalized + vec3(1.0));
}
