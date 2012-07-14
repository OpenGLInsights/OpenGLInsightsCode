#version 150 core
uniform sampler2D unit_input;	//must be linearly sampled
uniform vec4 dir;		//blur direction

//the kernel is actually 9x9
const vec3 offset = vec3( 0.0, 1.3846153846, 3.2307692308 );
const vec3 weight = vec3( 0.2255859375, 0.314208984375, 0.06982421875 );

out vec4 color;

void main(void)	{
	vec2 ds = vec2(1.0) / textureSize(unit_input,0);
	color = weight.x *
			texture( unit_input, ds*( gl_FragCoord.xy+dir.xy*offset.x ) )+ 
		weight.y *(
			texture( unit_input, ds*( gl_FragCoord.xy+dir.xy*offset.y ) )+
			texture( unit_input, ds*( gl_FragCoord.xy-dir.xy*offset.y ) ))+
		weight.z *(
			texture( unit_input, ds*( gl_FragCoord.xy+dir.xy*offset.z ) )+
			texture( unit_input, ds*( gl_FragCoord.xy-dir.xy*offset.z ) ));
}
