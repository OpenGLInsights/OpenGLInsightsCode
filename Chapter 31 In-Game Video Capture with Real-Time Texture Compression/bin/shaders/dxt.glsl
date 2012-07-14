#version 330
precision highp float;
precision highp int;

float saturate(float v) { return clamp(v, -1, 1); }
vec2 saturate(vec2 v) { return clamp(v, -1, 1); }
vec3 saturate(vec3 v) { return clamp(v, -1, 1); }
vec4 saturate(vec4 v) { return clamp(v, -1, 1); }


////////////////////////////////////////////////////////////////////////////////
const float offset = 128.0 / 255.0;//0.5;

/// RGBA ->CoCgAY
vec4 toCoCgAY(vec4 rgba)
{
    float Y  = (   rgba.r + 2*rgba.g + rgba.b ) * 0.25;
    float Co = ( 2*rgba.r - 2*rgba.b          ) * 0.25 + offset;
    float Cg = (  -rgba.r + 2*rgba.g - rgba.b ) * 0.25 + offset;

    return vec4(Co, Cg, rgba.a, Y);
}

/// RGBX -> CoCgXY, chroma divided by Y
vec4 toCoCgXY(vec4 rgbx)
{
    float Y= rgbx.r + 2*rgbx.g + rgbx.b;   //0..4
    float rY= 1.0 / max(1.0/1020, Y);
    float Co= (rgbx.r + rgbx.g)*rY;
    float Cg= 2*rgbx.g*rY;

    return vec4(Co, Cg, rgbx.w, 0.25*Y);
}

void ExtractColorBlockCoCgAY(
    out vec4 col[16] ,
    sampler2D image,
    ivec2 coord,
    int mip)
{
    for (int i= 0; i<4; i++) {
        for (int j= 0; j<4; j++) {
            vec4 color= texelFetch(image, coord + ivec2(j,i), mip);
            //color = sqrt(color);
            col[i*4+j] = toCoCgAY(color);
        }
    }
}

void ExtractColorBlockCoCgXY(
    out vec4 col[16] ,
    sampler2D image,
    ivec2 coord,
    int mip)
{
    for (int i= 0; i<4; i++) {
        for (int j= 0; j<4; j++) {
            vec4 color= texelFetch(image, coord + ivec2(j,i), mip);
            col[i*4+j] = toCoCgXY(color);
        }
    }
}

void ExtractColorBlockRGBA(
    out vec4 col[16] ,
    sampler2D image,
    ivec2 coord,
    int mip)
{
    for (int i= 0; i<4; i++) {
        for (int j= 0; j<4; j++) {
            vec4 color= texelFetch(image, coord + ivec2(j,i), mip);
            col[i*4+j] = color;//vec4(sqrt(color.rgb), color.a);
        }
    }
}

void ExtractColorBlockRGBA_MS(
    out vec4 col[16] ,
    sampler2DMS image,
	int nsamples,
    ivec2 coord)
{
	vec4 invn= vec4(1.0/float(nsamples));
    for (int i= 0; i<4; i++) {
        for (int j= 0; j<4; j++) {
			vec4 color= vec4(0);
			for(int k= 0; k<nsamples; ++k)
				color += invn*texelFetch(image, coord + ivec2(j,i), k);
            col[i*4+j] = color;//sqrt(color);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// find minimum and maximum colors based on bounding box in color space
void FindMinMaxColorsBox(vec4 block[16] , out vec4 mincol, out vec4 maxcol)
{
    mincol = block[0];
    maxcol = block[0];

    for (int i= 1; i < 16; i++) {
        mincol = min(mincol, block[i]);
        maxcol = max(maxcol, block[i]);
    }
}

void SelectRGBDiagonal(vec4 block[16] , inout vec3 mincol, inout vec3 maxcol)
{
    vec3 center= (mincol + maxcol) * 0.5;
    
    vec2 cov= vec2(0);
    for (int i= 0; i < 16; i++)
    {
        vec3 t= block[i].rgb - center;
        cov.x += t.x * t.z;
        cov.y += t.y * t.z;
    }

    if (cov.x < 0) {
        float temp= maxcol.x;
        maxcol.x = mincol.x;
        mincol.x = temp;
    }
    if (cov.y < 0) {
        float temp= maxcol.y;
        maxcol.y = mincol.y;
        mincol.y = temp;
    }
}

uint GetCoCgScale(vec2 minColor, vec2 maxColor)
{
    vec2 m0= abs(minColor - offset);
    vec2 m1= abs(maxColor - offset);

    float m= max(max(m0.x, m0.y), max(m1.x, m1.y));

    const float s0= 64.0 / 255.0;
    const float s1= 32.0 / 255.0;

    uint scale= 1U;
    if (m < s0) scale = 2U;
    if (m < s1) scale = 4U;
    
    return scale;
}

void SelectCoCgDiagonal(vec4 block[16] , inout vec2 minColor, inout vec2 maxColor)
{
    vec2 mid= (maxColor + minColor) * 0.5;
    
    float cov= 0;
    for (int i= 0; i < 16; i++)
    {
        vec2 t= block[i].xy - mid;
        cov += t.x * t.y;
    }
    if (cov < 0) {
        float tmp= maxColor.y;
        maxColor.y = minColor.y;
        minColor.y = tmp;
    }
}

uint EmitEndPointsCoCg(inout vec2 mincol, inout vec2 maxcol, uint scale)
{
    maxcol = (maxcol - offset) * float(scale) + offset;
    mincol = (mincol - offset) * float(scale) + offset;
    
    vec2 inset = (maxcol - mincol)/16 - (8/255.0)/16;
    mincol = saturate(mincol + inset);
    maxcol = saturate(maxcol - inset);

    uvec2 imaxcol = uvec2(round(maxcol * vec2(31, 63)));
    uvec2 imincol = uvec2(round(mincol * vec2(31, 63)));

    uvec2 OUT;
    OUT.x = (imaxcol.r << 11U) | (imaxcol.g << 5U) | (scale - 1U);
    OUT.y = (imincol.r << 11U) | (imincol.g << 5U) | (scale - 1U);

    imaxcol.r = (imaxcol.r << 3U) | (imaxcol.r >> 2U);
    imaxcol.g = (imaxcol.g << 2U) | (imaxcol.g >> 4U);
    imincol.r = (imincol.r << 3U) | (imincol.r >> 2U);
    imincol.g = (imincol.g << 2U) | (imincol.g >> 4U);

    maxcol = vec2(imaxcol) * (1.0/255);
    mincol = vec2(imincol) * (1.0/255);

    // Undo rescale.
    maxcol = (maxcol - offset) / float(scale) + offset;
    mincol = (mincol - offset) / float(scale) + offset;

    // We have to do this in case we select an alternate diagonal.
    if(OUT.x < OUT.y)
    {
        vec2 tmp = mincol.rg;
        mincol.rg = maxcol.rg;
        maxcol.rg = tmp;
        return OUT.y | (OUT.x << 16U);
    }

    return OUT.x | (OUT.y << 16U);
}

uint EmitIndicesCoCg(vec4 block[16] , vec2 mincol, vec2 maxcol)
{
    const float COCG_RANGE= 3;

    vec2 dir = (maxcol - mincol);
    vec2 origin = maxcol + dir / (2.0 * COCG_RANGE);
    dir /= dot(dir, dir);

    // Compute indices
    uint indices= 0U;
    for (int i= 0; i < 16; i++)
    {
        uint index= uint(saturate(dot(origin - block[i].xy, dir)) * COCG_RANGE);
        indices |= index << uint(i * 2);
    }

    uint i0 = (indices & 0x55555555U);
    uint i1 = (indices & 0xAAAAAAAAU) >> 1U;
    indices = ((i0 ^ i1) << 1U) | i1;

    // Output indices
    return indices;
}

uint EmitEndPointsAlpha(inout float mincol, inout float maxcol)
{
    float inset = (maxcol - mincol) / 32.0 - (16.0/255) / 32.0;
    mincol = saturate(mincol + inset);
    maxcol = saturate(maxcol - inset);

    uint c0 = uint(round(mincol * 255));
    uint c1 = uint(round(maxcol * 255));

    return (c0 << 8U) | c1;
}

////////////////////////////////////////////////////////////////////////////////
// Optimized index selection.
uvec2 EmitAlphaIndicesYCoCgDXT5(vec4 block[16] , float minAlpha, float maxAlpha)
{
    const int ALPHA_RANGE= 7;

    float bias= maxAlpha + (maxAlpha - minAlpha) / (2.0 * ALPHA_RANGE);
    float scale= 1.0f / (maxAlpha - minAlpha);

    uvec2 indices= uvec2(0U);

    for (int i= 0; i < 6; i++)
    {
        uint index= uint(saturate((bias - block[i].a) * scale) * ALPHA_RANGE);
        indices.x |= index << uint(3 * i);
    }

    for (int i= 6; i < 16; i++)
    {
        uint index= uint(saturate((bias - block[i].a) * scale) * ALPHA_RANGE);
        indices.y |= index << uint(3 * i - 18);
    }

    uvec2 i0= (indices >> 0U) & 0x09249249U;
    uvec2 i1= (indices >> 1U) & 0x09249249U;
    uvec2 i2= (indices >> 2U) & 0x09249249U;

    i2 ^= i0 & i1;
    i1 ^= i0;
    i0 ^= (i1 | i2);

    indices.x = (i2.x << 2U) | (i1.x << 1U) | i0.x;
    indices.y = (((i2.y << 2U) | (i1.y << 1U) | i0.y) << 2U) | (indices.x >> 16U);
    indices.x <<= 16U;

    return indices;
}

uvec2 EmitIndicesAlpha(vec4 block[16] , float minAlpha, float maxAlpha)
{
    const int ALPHA_RANGE= 7;

    float bias= maxAlpha + (maxAlpha - minAlpha) / (2.0 * ALPHA_RANGE);
    float scale= 1.0f / (maxAlpha - minAlpha);

    uvec2 indices= uvec2(0U);

    for (int i= 0; i < 6; i++)
    {
        uint index= uint(saturate((bias - block[i].a) * scale) * ALPHA_RANGE);
        indices.x |= index << uint(3 * i);
    }

    for (int i= 6; i < 16; i++)
    {
        uint index= uint(saturate((bias - block[i].a) * scale) * ALPHA_RANGE);
        indices.y |= index << uint(3 * i - 18);
    }

    uvec2 i0= (indices >> 0U) & 0x09249249U;
    uvec2 i1= (indices >> 1U) & 0x09249249U;
    uvec2 i2= (indices >> 2U) & 0x09249249U;

    i2 ^= i0 & i1;
    i1 ^= i0;
    i0 ^= (i1 | i2);

    indices.x = (i2.x << 2U) | (i1.x << 1U) | i0.x;
    indices.y = (((i2.y << 2U) | (i1.y << 1U) | i0.y) << 2U) | (indices.x >> 16U);
    indices.x <<= 16U;

    return indices;
}
 


vec3 RoundAndExpand(vec3 v, out uint w)
{
    ivec3 c = ivec3(round(v * vec3(31, 63, 31)));
    w = uint((c.r << 11) | (c.g << 5) | c.b);

    c.rb = (c.rb << 3) | (c.rb >> 2);
    c.g = (c.g << 2) | (c.g >> 4);

    return vec3(c * (1.0/255));
}

uint EmitEndPointsDXT1(inout vec4 mincol, inout vec4 maxcol)
{
    vec4 inset = (maxcol - mincol)/16 - (8/255.0)/16;
    mincol = saturate(mincol + inset);
    maxcol = saturate(maxcol - inset);

    uvec2 OUT;
    maxcol.rgb = RoundAndExpand(maxcol.rgb, OUT.x);
    mincol.rgb = RoundAndExpand(mincol.rgb, OUT.y);

    // We have to do this in case we select an alternate diagonal.
    if(OUT.x < OUT.y)
    {
        vec3 tmp = mincol.rgb;
        mincol.rgb = maxcol.rgb;
        maxcol.rgb = tmp;
        return OUT.y | (OUT.x << 16U);
    }

    return OUT.x | (OUT.y << 16U);
}

uint EmitEndPointsDXT1_alpha(inout vec4 mincol, inout vec4 maxcol)
{
    vec4 inset = (maxcol - mincol)/16 - (8.0/255)/16;
    mincol = saturate(mincol + inset);
    maxcol = saturate(maxcol - inset);

    uvec2 OUT;
    maxcol.rgb = RoundAndExpand(maxcol.rgb, OUT.x);
    mincol.rgb = RoundAndExpand(mincol.rgb, OUT.y);

    // We have to do this in case we select an alternate diagonal.
    bool alpha= mincol.a < 0.5;
    if(OUT.x < OUT.y ^^ alpha) {
        vec3 tmp= mincol.rgb;
        mincol.rgb = maxcol.rgb;
        maxcol.rgb = tmp;
        return OUT.y | (OUT.x << 16U);
    }

    return OUT.x | (OUT.y << 16U);
}

uint EmitIndicesDXT1_noalpha(vec4 block[16] , vec4 mincol, vec4 maxcol)
{
    const float RGB_RANGE= 3;  //divisions of min-max range

    vec3 dir= (maxcol.rgb - mincol.rgb);
    vec3 origin= maxcol.rgb + dir.rgb / (2.0 * RGB_RANGE);   //half the division above max
    dir /= dot(dir, dir);

    // Compute indices
    uint indices= 0U;
    for (int i= 0; i < 16; i++)
    {
        uint index= uint(saturate(dot(origin - block[i].rgb, dir)) * RGB_RANGE);
        indices |= index << uint(i * 2);
    }

    uint i0= (indices & 0x55555555U);
    uint i1= (indices & 0xAAAAAAAAU) >> 1U;
    indices = ((i0 ^ i1) << 1U) | i1;

    // Output indices
    return indices;
}

///DXT1 with alpha
uint EmitIndicesDXT1_alpha(vec4 block[16] , vec4 mincol, vec4 maxcol)
{
    const float RGB_RANGE= 2;  //divisions of min-max range

    vec3 dir= (maxcol.rgb - mincol.rgb);
    vec3 origin= maxcol.rgb + dir.rgb / (2.0 * RGB_RANGE);   //half the division above max
    dir /= dot(dir, dir);

    // Compute indices
    uint indices= 0U;
    for (int i= 0; i < 16; i++)
    {
        uint index= block[i].a < 0.5
            ? 3U
            : min(2U, uint(saturate(dot(origin - block[i].rgb, dir)) * RGB_RANGE));
        indices |= index << uint(i * 2);
    }

    uint i0= (indices & 0x55555555U) << 1U;
    uint i1= (indices & 0xAAAAAAAAU) >> 1U;
    indices = i0 | i1;

    // Output indices
    return indices;
}

////////////////////////////////////////////////////////////////////////////////

#define MODE_DXT5       0
#define MODE_YCOCG      1
#define MODE_YCOCGS     2
#define MODE_COCGXY     3

// compress a 4x4 block to DXT5 format
// integer version, renders to 4 x int32 buffer
uniform sampler2D image;
uniform int mip = 0;
uniform int mode;

out uvec4 _retval;

void main()
{
    ivec2 co= ivec2(4 * gl_FragCoord.xy) & ivec2(~3);

    // read block
    vec4 block[16] ;

    if(mode == MODE_DXT5)
        ExtractColorBlockRGBA(block, image, co, mip);
    else if(mode == MODE_YCOCG || mode == MODE_YCOCGS)
        ExtractColorBlockCoCgAY(block, image, co, mip);
    else if(mode == MODE_COCGXY)
        ExtractColorBlockCoCgXY(block, image, co, mip);

    // find min and max colors
    vec4 mincol, maxcol;
    FindMinMaxColorsBox(block, mincol, maxcol);


    uvec4 OUT;
    if(mode == MODE_DXT5)
    {
        SelectRGBDiagonal(block, mincol.rgb, maxcol.rgb);

        // Output RGB in DXT1 block.
        OUT.z = EmitEndPointsDXT1(mincol, maxcol);
        OUT.w = EmitIndicesDXT1_noalpha(block, mincol, maxcol);
    }
    else if(mode == MODE_YCOCGS)
    {
        SelectCoCgDiagonal(block, mincol.xy, maxcol.xy);

        uint scale = 1U;//GetCoCgScale(mincol.xy, maxcol.xy);

        // Output CoCg in DXT1 block.
        OUT.z = EmitEndPointsCoCg(mincol.xy, maxcol.xy, scale);
        OUT.w = EmitIndicesCoCg(block, mincol.xy, maxcol.xy);
    }
    else
    {
        SelectCoCgDiagonal(block, mincol.xy, maxcol.xy);

        // Output CoCg in DXT1 block.
        OUT.z = EmitEndPointsDXT1(mincol, maxcol);
        OUT.w = EmitIndicesDXT1_noalpha(block, mincol, maxcol);
    }

    // Output Y in DXT5 alpha block.
    OUT.x = EmitEndPointsAlpha(mincol.a, maxcol.a);

    uvec2 indices= EmitIndicesAlpha(block, mincol.a, maxcol.a);
    OUT.x |= indices.x;
    OUT.y = indices.y;

    _retval = OUT;
}
