#version 330
precision highp float;
precision highp int;

///
vec3 sample(
    sampler2DMS texfb,
    ivec2 coord,
    int nsamples)
{
	vec3 c= vec3(0);
	for( int i= 0; i<nsamples; ++i )
		c += texelFetch(texfb,coord,i).xyz;

	return c;
}

const float offset = 128.0 / 255.0;

const vec3 TO_Y = vec3(0.2215, 0.7154, 0.0721);
const vec3 TO_U = vec3(-0.1145, -0.3855, 0.5000);
const vec3 TO_V = vec3(0.5016, -0.4556, -0.0459);

void ExtractColorBlockYY(
    out vec2 col[16] ,
    sampler2D image,
    ivec2 coord)
{

    for(int i=0; i<4; i++) {
        for(int j=0; j<4; j++) {
            vec3 color = texelFetch(image, coord + ivec2(j,i), 0).xyz;
            col[i*4+j].x = dot(TO_Y, color);
        }
    }

    for(int i=0; i<4; i++) {
        for(int j=4; j<8; j++) {
            vec3 color = texelFetch(image, coord + ivec2(j,i), 0).xyz;
            col[i*4+j-4].y = dot(TO_Y, color);
        }
    }
}

void ExtractColorBlockUV(
    out vec2 col[16] ,
    sampler2D image,
    ivec2 coord)
{
    for (int i= 0; i<4; i++) {
        for (int j= 0; j<8; j+=2) {
            vec3 color0 = texelFetch(image, coord + ivec2(j,i), 0).xyz;
            vec3 color1 = texelFetch(image, coord + ivec2(j+1,i), 0).xyz;
            vec3 color = 0.5 * (color0 + color1);
            col[i*4+(j>>1)].x = dot(TO_U, color) + offset;
            col[i*4+(j>>1)].y = dot(TO_V, color) + offset;
        }
    }
}

// find minimum and maximum colors based on bounding box in color space
void FindMinMaxBox(vec2 block[16] , out vec2 mincol, out vec2 maxcol)
{
    mincol = block[0];
    maxcol = block[0];
    
    for (int i= 1; i < 16; i++) {
        mincol = min(mincol, block[i]);
        maxcol = max(maxcol, block[i]);
    }
}

uvec2 EmitValueEndPoints(inout vec2 mincol, inout vec2 maxcol)
{
    vec2 inset= (maxcol - mincol) / 32.0 - (16.0 / 255.0) / 32.0;
    mincol = saturate(mincol + inset);
    maxcol = saturate(maxcol - inset);
    
    uvec2 c0= uvec2(round(mincol * 255));
    uvec2 c1= uvec2(round(maxcol * 255));
    
    return (c0 << 8U) | c1;
}

// Optimized index selection
uvec4 EmitValueIndices(vec2 block[16] , vec2 minv, vec2 maxv)
{
    const int ALPHA_RANGE= 7;

    vec2 bias= maxv + (maxv - minv) / (2.0 * ALPHA_RANGE);
    vec2 scale= 1.0f / (maxv - minv);

    uvec4 indices= uvec4(0U);

    for (int i= 0; i < 6; i++)
    {
        uvec2 index= uvec2(saturate((bias - block[i]) * scale) * ALPHA_RANGE);
        indices.xz |= index << uint(3 * i);
    }

    for (int i= 6; i < 16; i++)
    {
        uvec2 index= uvec2(saturate((bias - block[i]) * scale) * ALPHA_RANGE);
        indices.yw |= index << uint(3 * i - 18);
    }

    uvec4 i0= (indices >> 0U) & 0x09249249U;
    uvec4 i1= (indices >> 1U) & 0x09249249U;
    uvec4 i2= (indices >> 2U) & 0x09249249U;

    i2 ^= i0 & i1;
    i1 ^= i0;
    i0 ^= (i1 | i2);

    indices.xz = (i2.xz << 2U) | (i1.xz << 1U) | i0.xz;
    indices.yw = (((i2.yw << 2U) | (i1.yw << 1U) | i0.yw) << 2U) | (indices.xz >> 16U);
    indices.xz <<= 16U;

    return indices;
}

uvec4 encodeDXT( vec2 block[16] )
{
    // find min and max colors
    vec2 mincol, maxcol;
    FindMinMaxBox(block, mincol, maxcol);

    uvec4 OUT;
    OUT.xz = EmitValueEndPoints(mincol, maxcol);

    uvec4 indices= EmitValueIndices(block, mincol, maxcol);
    OUT.xz |= indices.xz;
    OUT.yw = indices.yw;

    return OUT;
}


// compress a 4x4 block to YUYV-DXT5 format
// integer version, renders to 4 x int32 buffer
uniform sampler2D image;
out uint4 retval;

void main()
{
    ivec2 co= ivec2(4 * gl_FragCoord.xy);//int2(4 * coord * sz);
    ivec2 cb= co & ivec2(~7,~3);
    bool Y = co.x - cb.x < 4;

    vec2 block[16] ;

    if(Y)
        ExtractColorBlockYY(block, image, cb);
    else
        ExtractColorBlockUV(block, image, cb);

    retval = encodeDXT(block);
} 