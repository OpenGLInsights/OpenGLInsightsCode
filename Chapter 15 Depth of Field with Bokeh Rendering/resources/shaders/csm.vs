#version 420 core

#ifdef CSM_RENDERER
layout(location = ATTR_POSITION) in  vec2 Position;

void main()
{
	gl_Position  = vec4(Position,0,1);
}
#endif

#ifdef CSM_FILTER
layout(location = ATTR_POSITION) in  vec2 Position;
out int vCascadeLayer;
void main()
{
	vCascadeLayer = gl_InstanceID;
	gl_Position  = vec4(Position,0,1);
}
#endif
