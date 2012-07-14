
#include "dds.h"
#include <base/base.h>
#include <stdio.h>

typedef unsigned int uint;
typedef unsigned char uchar;

#define MAKEFOURCC(ch0,ch1,ch2,ch3) \
	((uint)(uchar)(ch0)|((uint)(uchar)(ch1)<< 8)| \
	((uint)(uchar)(ch2)<<16)|((uint)(uchar)(ch3)<<24))

#define ISBITMASK( r,g,b,a ) ( ddpf.dwRBitMask==r && ddpf.dwGBitMask==g && ddpf.dwBBitMask==b && ddpf.dwRGBAlphaBitMask==a )

#define DDS_MAGIC 0x20534444

typedef struct  {
    uint dwSize;
    uint dwFlags;
    uint dwFourCC;
    uint dwRGBBitCount;
    uint dwRBitMask;
	uint dwGBitMask;
	uint dwBBitMask;
    uint dwRGBAlphaBitMask;
} DDPIXELFORMAT;

typedef struct  {
    uint dwCaps1;
    uint dwCaps2;
    uint Reserved[2];
} DDSCAPS2;

typedef struct  {
    uint dwSize;
    uint dwFlags;
    uint dwHeight;
    uint dwWidth;
    uint dwPitchOrLinearSize;
    uint dwDepth;
    uint dwMipMapCount;
    uint dwReserved1[11];
    DDPIXELFORMAT ddpfPixelFormat;
    DDSCAPS2 ddsCaps;
    uint dwReserved2;
} DDSURFACEDESC2;

struct dds_hdr {
	uint _magic;
	DDSURFACEDESC2 _ddsd;
};

typedef struct  {
    uint dxgiFormat;
    uint resourceDimension;
    uint miscFlag;
    uint arraySize;
    uint reserved;
} DDS_HEADER_DXT10;

struct dds_hdr_dx10 {
	uint _magic;
	DDSURFACEDESC2 _ddsd;
	DDS_HEADER_DXT10 _dx10;
};

//  DDS_header.sPixelFormat.dwFlags
enum EDDSPFFlags {
	DDPF_ALPHAPIXELS=0x00000001,
    DDPF_ALPHA      =0x00000002,
	DDPF_FOURCC		=0x00000004,
	DDPF_INDEXED	=0x00000020,
	DDPF_RGB		=0x00000040,
    DDPF_YUV        =0x00000200,
    DDPF_LUMINANCE  =0x00020000,
};

enum EDDSCaps {
	DDSCAPS_COMPLEX	=0x000008,
	DDSCAPS_MIPMAP	=0x400000,
	DDSCAPS_TEXTURE	=0x001000,
};

enum EDDSFlags {
	DDSD_CAPS		=0x000001,
	DDSD_HEIGHT		=0x000002,
	DDSD_WIDTH		=0x000004,
	DDSD_PITCH		=0x000008,
	DDSD_PIXELFORMAT=0x001000,
	DDSD_MIPMAPCOUNT=0x020000,
	DDSD_LINEARSIZE	=0x080000,
	DDSD_DEPTH		=0x800000,
};

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

static bool get_tex2d_desc(const dds_hdr* dhdr, int* w, int* h, int* mips, base::pixelfmt* pixfmt)
{
	if(dhdr->_magic != DDS_MAGIC)
		throw base::exception("Bad magic number in DDS file!");

    const DDSURFACEDESC2 *hdr = &dhdr->_ddsd;
	const DDPIXELFORMAT &ddpf = hdr->ddpfPixelFormat;

	// fill other stuff in desc
	*w = hdr->dwWidth;
	*h = hdr->dwHeight;
	*mips = hdr->dwMipMapCount>1 ? hdr->dwMipMapCount : 1U;

	// find pixel format
	*pixfmt = base::PF_UNKNOWN;
	if( ddpf.dwFlags & DDPF_RGB ) {
		if( 32==ddpf.dwRGBBitCount ) {
			if( ISBITMASK(0x00ff0000,0x0000ff00,0x000000ff,0xff000000) )
				*pixfmt = base::PF_BGRA8;
			if( ISBITMASK(0x000000ff,0x0000ff00,0x00ff0000,0xff000000) )
				*pixfmt = base::PF_RGBA8;
		}
	}
	else if( ddpf.dwFlags & DDPF_FOURCC ) {
		if( MAKEFOURCC('D','X','T','1')==ddpf.dwFourCC )
			*pixfmt = base::PF_DXT1;
		else if( MAKEFOURCC('D','X','T','3')==ddpf.dwFourCC )
			*pixfmt = base::PF_DXT3;
		else if( MAKEFOURCC('D','X','T','5')==ddpf.dwFourCC )
			*pixfmt = base::PF_DXT5;
		else if( MAKEFOURCC(114,0,0,0)==ddpf.dwFourCC )
			*pixfmt = base::PF_R32F;
		else if( MAKEFOURCC(115,0,0,0)==ddpf.dwFourCC )
			*pixfmt = base::PF_RG32F;
		else if( MAKEFOURCC(116,0,0,0)==ddpf.dwFourCC )
			*pixfmt = base::PF_RGBA32F;
		else if( MAKEFOURCC('D','X','1','0')==ddpf.dwFourCC )
			return false;
	}
	else if( ddpf.dwFlags & DDPF_LUMINANCE ) {
        if( ddpf.dwRGBBitCount==8 ) {
            *pixfmt = base::PF_R8;
        }
    }
	else
		return false;

	return true;
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

static uint get_byte_size(int w, int h, int mips, base::pixelfmt fmt)
{
    int wm=w, hm=h, size=0;
    for(int i=0; i<mips; ++i)
    {
        size += wm*hm;

        wm >>= 1;
        hm >>= 1;
        if(wm==0) wm=1;
        if(hm==0) hm=1;
    }

    const base::pfd* pf = base::get_pfd(fmt);
    
    return size * pf->_size;
}


void* dds::get_texture_data(const char* file, int* w, int* h, int* mips, base::pixelfmt* pixfmt)
{
    FILE* fp = fopen(file, "rb");
    if(!fp) throw base::exception("file not found");

    dds_hdr hdr;
    fread(&hdr, sizeof(hdr), 1, fp);

    if(!get_tex2d_desc(&hdr, w, h, mips, pixfmt))
        return 0;

    int size = get_byte_size(*w, *h, *mips, *pixfmt);
    void* data = ::malloc(size);

    fread(data, size, 1, fp);
    fclose(fp);

    return data;
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

struct ddsd2pf {
    base::pixelfmt _pf;
	DDPIXELFORMAT _ddpf;
};

const ddsd2pf __tbl[]={
	{ base::PF_BGRA8,		{ 32,DDPF_RGB,0,32,0x00ff0000,0x0000ff00,0x000000ff,0xff000000 } },
	{ base::PF_RGBA8,		{ 32,DDPF_RGB,0,32,0x000000ff,0x0000ff00,0x00ff0000,0xff000000 } },
	{ base::PF_RGB8,		{ 32,DDPF_RGB,0,32,0x000000ff,0x0000ff00,0x00ff0000,0x00000000 } },
	{ base::PF_BGR8,		{ 32,DDPF_RGB,0,32,0x00ff0000,0x0000ff00,0x000000ff,0x00000000 } },

	{ base::PF_DXT1,		{ 32,DDPF_FOURCC,MAKEFOURCC('D','X','T','1'),0,0,0,0,0 } },
	{ base::PF_DXT3,		{ 32,DDPF_FOURCC,MAKEFOURCC('D','X','T','3'),0,0,0,0,0 } },
	{ base::PF_DXT5,		{ 32,DDPF_FOURCC,MAKEFOURCC('D','X','T','5'),0,0,0,0,0 } },

	{ base::PF_R32F,		{ 32,DDPF_FOURCC,MAKEFOURCC(114,0,0,0),0,0,0,0,0 } },
	{ base::PF_RG32F,		{ 32,DDPF_FOURCC,MAKEFOURCC(115,0,0,0),0,0,0,0,0 } },
	{ base::PF_RGBA32F,	{ 32,DDPF_FOURCC,MAKEFOURCC(116,0,0,0),0,0,0,0,0 } },
};

bool fill_pixel_format(DDPIXELFORMAT *ddpf, const base::pixelfmt pf)
{
	const uint n = sizeof(__tbl)/sizeof(ddsd2pf);

	for(uint i=0; i<n; ++i) {
		if( __tbl[i]._pf==pf ) {
			memcpy(ddpf, &__tbl[i]._ddpf, sizeof(*ddpf));
			return true;
		}
	}
	return false;
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

bool dds::save_texture(const char* file, int w, int h, int mips, base::pixelfmt fmt, const void* data)
{
	if( (NULL == file) ||
		(w < 1) || (h < 1) ||
		(data == NULL ) )
		return false;

    dds_hdr hdr;
    hdr._magic = DDS_MAGIC;
    DDSURFACEDESC2* surf = &hdr._ddsd;

    int size = get_byte_size(w, h, mips, fmt);

	//
	memset(surf, 0, sizeof(DDSURFACEDESC2));
	surf->dwSize = 124;
	surf->dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT | DDSD_LINEARSIZE;
    if(mips>1)
        surf->dwFlags |= DDSD_MIPMAPCOUNT;
	surf->dwWidth = w;
	surf->dwHeight = h;
	surf->dwPitchOrLinearSize = size;
    surf->dwMipMapCount = mips;

    if(!fill_pixel_format(&surf->ddpfPixelFormat, fmt))
        return false;

	surf->ddsCaps.dwCaps1 = DDSCAPS_TEXTURE;
    if(mips>1)
        surf->ddsCaps.dwCaps1 |= DDSCAPS_MIPMAP | DDSCAPS_COMPLEX;

	//write to file
	FILE *fp = fopen(file, "wb");
	fwrite(&hdr, sizeof(hdr), 1, fp);

	fwrite(data, 1, size, fp);
	fclose(fp);

    return true;
}
