#version 330 core

precision highp int;
precision highp float;

layout(lines) in;
layout(triangle_strip, max_vertices = 16) out;	// Too much!! in-shader culling?

/*in block
{
//	vec4 Position;
} In[]*/
out block
{
	flat vec3 p1;
	flat vec3 p2;
	smooth vec3 fragViewPos;
} Out;

uniform mat4 mvMat;
uniform mat4 pMat;
uniform float radius;
uniform sampler2D gradientTexture;



void main()
{	
	vec3 tmp;
	const vec3 _Y_ = vec3(0.0,1.0,0.0);
	const vec3 _X_ = vec3(1.0,0.0,0.0);
	
	//read line vertices
	vec3 v1 = gl_in[0].gl_Position.xyz;
	vec3 v2 = gl_in[1].gl_Position.xyz;

	//
	Out.p1=v1;						//view space line point1 to FP
	Out.p2=v2;						//view space line point2 to FP
	vec3 lineDir = v2-v1;
	float lineLength=length(v1-v2);	//line length to FP
	vec3 normLineDir = lineDir/lineLength;

	//construction of the hortonormal basis of the line
	vec3 d2=cross(normLineDir,_Y_);
	if(abs(normLineDir.y)>0.999)
		d2=cross(normLineDir,_X_);
	d2=normalize(d2);
	vec3 d3=cross(normLineDir,d2);//don't need to normalize here
	vec3 d2norm=d2;
	vec3 d3norm=d3;
	d2*=radius;
	d3*=radius;

	vec3 lineDirOffsetM = radius*normLineDir;
	vec3 lineDirOffsetP = lineDir + lineDirOffsetM;

	//precompute all vertices position
	vec4 viewPos000 = vec4( v1 -d2-d3 -lineDirOffsetM, 1.0);
	vec4 viewPos001 = vec4( v1 -d2+d3 -lineDirOffsetM, 1.0);
	vec4 viewPos010 = vec4( v1 +d2-d3 -lineDirOffsetM, 1.0);
	vec4 viewPos011 = vec4( v1 +d2+d3 -lineDirOffsetM, 1.0);
	vec4 viewPos110 = vec4( v1 +d2-d3 +lineDirOffsetP, 1.0);
	vec4 viewPos111 = vec4( v1 +d2+d3 +lineDirOffsetP, 1.0);
	vec4 viewPos100 = vec4( v1 -d2-d3 +lineDirOffsetP, 1.0);
	vec4 viewPos101 = vec4( v1 -d2+d3 +lineDirOffsetP, 1.0);
	//... and their projection
	vec4 viewPos000proj = pMat * viewPos000;
	vec4 viewPos001proj = pMat * viewPos001;
	vec4 viewPos011proj = pMat * viewPos011;
	vec4 viewPos010proj = pMat * viewPos010;
	vec4 viewPos100proj = pMat * viewPos100;
	vec4 viewPos101proj = pMat * viewPos101;
	vec4 viewPos111proj = pMat * viewPos111;
	vec4 viewPos110proj = pMat * viewPos110;

	Out.fragViewPos = viewPos001.xyz;
	gl_Position = viewPos001proj;
	EmitVertex();/////////////////////////////////////
	Out.fragViewPos = viewPos000.xyz;
	gl_Position = viewPos000proj;
	EmitVertex();/////////////////////////////////////
	Out.fragViewPos = viewPos101.xyz;
	gl_Position = viewPos101proj;
	EmitVertex();/////////////////////////////////////
	Out.fragViewPos = viewPos100.xyz;
	gl_Position = viewPos100proj;
	EmitVertex();/////////////////////////////////////
	Out.fragViewPos = viewPos111.xyz;
	gl_Position = viewPos111proj;
	EmitVertex();/////////////////////////////////////
	Out.fragViewPos = viewPos110.xyz;
	gl_Position = viewPos110proj;
	EmitVertex();/////////////////////////////////////
	Out.fragViewPos = viewPos011.xyz;
	gl_Position = viewPos011proj;
	EmitVertex();/////////////////////////////////////
	Out.fragViewPos = viewPos010.xyz;
	gl_Position = viewPos010proj;
	EmitVertex();/////////////////////////////////////
    EndPrimitive();//////////////////////////////////////////////////////////////////////////
	
	Out.fragViewPos = viewPos101.xyz;
	gl_Position = viewPos101proj;
	EmitVertex();/////////////////////////////////////
	Out.fragViewPos = viewPos111.xyz;
	gl_Position = viewPos111proj;
	EmitVertex();/////////////////////////////////////
	Out.fragViewPos = viewPos001.xyz;
	gl_Position = viewPos001proj;
	EmitVertex();/////////////////////////////////////
	Out.fragViewPos = viewPos011.xyz;
	gl_Position = viewPos011proj;
	EmitVertex();/////////////////////////////////////
	Out.fragViewPos = viewPos000.xyz;
	gl_Position = viewPos000proj;
	EmitVertex();/////////////////////////////////////
	Out.fragViewPos = viewPos010.xyz;
	gl_Position = viewPos010proj;
	EmitVertex();/////////////////////////////////////
	Out.fragViewPos = viewPos100.xyz;
	gl_Position = viewPos100proj;
	EmitVertex();/////////////////////////////////////
	Out.fragViewPos = viewPos110.xyz;
	gl_Position = viewPos110proj;
	EmitVertex();/////////////////////////////////////
    EndPrimitive();//////////////////////////////////////////////////////////////////////////

}

