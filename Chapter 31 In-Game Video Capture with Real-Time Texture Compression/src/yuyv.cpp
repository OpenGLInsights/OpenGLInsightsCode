
 typedef struct Color16 {
    uint16_t b : 5;
    uint16_t g : 6;
    uint16_t r : 5;
 } Color16;
 
 typedef struct Color32 {
     uint8_t r;
     uint8_t g;
     uint8_t b;
     uint8_t a;
 } Color32;
 
// DXT1 block
typedef struct BlockDXT1
{
    Color16 col0;
    Color16 col1;

    uint8_t row[4];
} BlockDXT1;

static unsigned int DXT1_evaluatePalette(const BlockDXT1* ab, Color32 color_array[4])
{
	// Does bit expansion before interpolation.
	color_array[0].b = (ab->col0.b << 3) | (ab->col0.b >> 2);
	color_array[0].g = (ab->col0.g << 2) | (ab->col0.g >> 4);
	color_array[0].r = (ab->col0.r << 3) | (ab->col0.r >> 2);
	
	color_array[1].r = (ab->col1.r << 3) | (ab->col1.r >> 2);
	color_array[1].g = (ab->col1.g << 2) | (ab->col1.g >> 4);
	color_array[1].b = (ab->col1.b << 3) | (ab->col1.b >> 2);
	
	if( *(const uint16_t*)&ab->col0 > *(const uint16_t*)&ab->col1 ) {
		// Four-color block: derive the other two colors.
		color_array[2].r = (2 * color_array[0].r + color_array[1].r) / 3;
		color_array[2].g = (2 * color_array[0].g + color_array[1].g) / 3;
		color_array[2].b = (2 * color_array[0].b + color_array[1].b) / 3;
		
		color_array[3].r = (2 * color_array[1].r + color_array[0].r) / 3;
		color_array[3].g = (2 * color_array[1].g + color_array[0].g) / 3;
		color_array[3].b = (2 * color_array[1].b + color_array[0].b) / 3;
		
		return 4;
	}
	else {
		// Three-color block: derive the other color.
		color_array[2].r = (color_array[0].r + color_array[1].r) / 2;
		color_array[2].g = (color_array[0].g + color_array[1].g) / 2;
		color_array[2].b = (color_array[0].b + color_array[1].b) / 2;
		
		// Set all components to 0 to match DXT specs.
		color_array[3].r = color_array[2].r;
		color_array[3].g = color_array[2].g;
		color_array[3].b = color_array[2].b;
		
		return 3;
	}
}

static void DXT1_decode(const BlockDXT1* ab, Color32 block[16])
{
	// Decode color block.
	Color32 color_array[4];
	DXT1_evaluatePalette(ab, color_array);
	
	// Write color block
	for( int j = 0; j < 4; j++ ) {
		for( int i = 0; i < 4; i++ ) {
			uint8_t idx = (ab->row[j] >> (2 * i)) & 3;
			block[j*4 + i] = color_array[idx];
		}
	}	
}


// DXT5 block
typedef struct BlockDXT5Alpha
{
    uint64_t alpha0 : 8;
    uint64_t alpha1 : 8;
    uint64_t bits0 : 3;
    uint64_t bits1 : 3;
    uint64_t bits2 : 3;
    uint64_t bits3 : 3;
    uint64_t bits4 : 3;
    uint64_t bits5 : 3;
    uint64_t bits6 : 3;
    uint64_t bits7 : 3;
    uint64_t bits8 : 3;
    uint64_t bits9 : 3;
    uint64_t bitsA : 3;
    uint64_t bitsB : 3;
    uint64_t bitsC : 3;
    uint64_t bitsD : 3;
    uint64_t bitsE : 3;
    uint64_t bitsF : 3;
} BlockDXT5Alpha;

static void AlphaDXT5_evaluatePalette8(const BlockDXT5Alpha* ab, uint8_t alpha[8])
{
	// 8-alpha block:  derive the other six alphas.
	// Bit code 000 = alpha0, 001 = alpha1, others are interpolated.
	alpha[0] = ab->alpha0;
	alpha[1] = ab->alpha1;
	alpha[2] = (6 * alpha[0] + 1 * alpha[1]) / 7;	// bit code 010
	alpha[3] = (5 * alpha[0] + 2 * alpha[1]) / 7;	// bit code 011
	alpha[4] = (4 * alpha[0] + 3 * alpha[1]) / 7;	// bit code 100
	alpha[5] = (3 * alpha[0] + 4 * alpha[1]) / 7;	// bit code 101
	alpha[6] = (2 * alpha[0] + 5 * alpha[1]) / 7;	// bit code 110
	alpha[7] = (1 * alpha[0] + 6 * alpha[1]) / 7;	// bit code 111
}

static void AlphaDXT5_evaluatePalette6(const BlockDXT5Alpha* ab, uint8_t alpha[8])
{
	// 6-alpha block.
	// Bit code 000 = alpha0, 001 = alpha1, others are interpolated.
	alpha[0] = ab->alpha0;
	alpha[1] = ab->alpha1;
	alpha[2] = (4 * alpha[0] + 1 * alpha[1]) / 5;	// Bit code 010
	alpha[3] = (3 * alpha[0] + 2 * alpha[1]) / 5;	// Bit code 011
	alpha[4] = (2 * alpha[0] + 3 * alpha[1]) / 5;	// Bit code 100
	alpha[5] = (1 * alpha[0] + 4 * alpha[1]) / 5;	// Bit code 101
	alpha[6] = 0x00;							// Bit code 110
	alpha[7] = 0xFF;							// Bit code 111
}

static const unsigned char gamma_map[] = {
 0x00, 0x15, 0x1c, 0x22, 0x27, 0x2b, 0x2e, 0x32, 0x35, 0x38, 0x3b, 0x3d,
 0x40, 0x42, 0x44, 0x46, 0x48, 0x4a, 0x4c, 0x4e, 0x50, 0x52, 0x54, 0x55,
 0x57, 0x59, 0x5a, 0x5c, 0x5d, 0x5f, 0x60, 0x62, 0x63, 0x65, 0x66, 0x67,
 0x69, 0x6a, 0x6b, 0x6d, 0x6e, 0x6f, 0x70, 0x72, 0x73, 0x74, 0x75, 0x76,
 0x77, 0x78, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f, 0x80, 0x81, 0x82, 0x83,
 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
 0x90, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x97, 0x98, 0x99,
 0x9a, 0x9b, 0x9c, 0x9c, 0x9d, 0x9e, 0x9f, 0xa0, 0xa0, 0xa1, 0xa2, 0xa3,
 0xa4, 0xa4, 0xa5, 0xa6, 0xa7, 0xa7, 0xa8, 0xa9, 0xaa, 0xaa, 0xab, 0xac,
 0xad, 0xad, 0xae, 0xaf, 0xaf, 0xb0, 0xb1, 0xb2, 0xb2, 0xb3, 0xb4, 0xb4,
 0xb5, 0xb6, 0xb6, 0xb7, 0xb8, 0xb8, 0xb9, 0xba, 0xba, 0xbb, 0xbc, 0xbc,
 0xbd, 0xbe, 0xbe, 0xbf, 0xc0, 0xc0, 0xc1, 0xc2, 0xc2, 0xc3, 0xc3, 0xc4,
 0xc5, 0xc5, 0xc6, 0xc7, 0xc7, 0xc8, 0xc8, 0xc9, 0xca, 0xca, 0xcb, 0xcb,
 0xcc, 0xcd, 0xcd, 0xce, 0xce, 0xcf, 0xcf, 0xd0, 0xd1, 0xd1, 0xd2, 0xd2,
 0xd3, 0xd4, 0xd4, 0xd5, 0xd5, 0xd6, 0xd6, 0xd7, 0xd7, 0xd8, 0xd9, 0xd9,
 0xda, 0xda, 0xdb, 0xdb, 0xdc, 0xdc, 0xdd, 0xdd, 0xde, 0xdf, 0xdf, 0xe0,
 0xe0, 0xe1, 0xe1, 0xe2, 0xe2, 0xe3, 0xe3, 0xe4, 0xe4, 0xe5, 0xe5, 0xe6,
 0xe6, 0xe7, 0xe7, 0xe8, 0xe8, 0xe9, 0xe9, 0xea, 0xea, 0xeb, 0xeb, 0xec,
 0xec, 0xed, 0xed, 0xee, 0xee, 0xef, 0xef, 0xf0, 0xf0, 0xf1, 0xf1, 0xf2,
 0xf2, 0xf3, 0xf3, 0xf4, 0xf4, 0xf5, 0xf5, 0xf6, 0xf6, 0xf7, 0xf7, 0xf8,
 0xf8, 0xf9, 0xf9, 0xf9, 0xfa, 0xfa, 0xfb, 0xfb, 0xfc, 0xfc, 0xfd, 0xfd,
 0xfe, 0xfe, 0xff, 0xff, 
};

static void AlphaDXT5_decode(const BlockDXT5Alpha* ab, Color32 block[16])
{
	uint8_t alpha_array[8];
	uint8_t index_array[16];

	if (ab->alpha0 > ab->alpha1)
		AlphaDXT5_evaluatePalette8(ab, alpha_array);
	else
		AlphaDXT5_evaluatePalette6(ab, alpha_array);
	
	index_array[0x0] = ab->bits0;
	index_array[0x1] = ab->bits1;
	index_array[0x2] = ab->bits2;
	index_array[0x3] = ab->bits3;
	index_array[0x4] = ab->bits4;
	index_array[0x5] = ab->bits5;
	index_array[0x6] = ab->bits6;
	index_array[0x7] = ab->bits7;
	index_array[0x8] = ab->bits8;
	index_array[0x9] = ab->bits9;
	index_array[0xA] = ab->bitsA;
	index_array[0xB] = ab->bitsB;
	index_array[0xC] = ab->bitsC;
	index_array[0xD] = ab->bitsD;
	index_array[0xE] = ab->bitsE;
	index_array[0xF] = ab->bitsF;

    for (int i = 0; i < 16; i++) {
        block[i].a = alpha_array[index_array[i]];
    }
}

/**
  @param col_shift byte shift between columns
  @param row_shift byte shift from behind the last column in one row to the first column in next row
**/
static void AlphaDXT5_decode2(const BlockDXT5Alpha* ab, uint8_t* dst, int col_shift, int row_shift)
{
	uint8_t alpha_array[8];
	uint8_t index_array[16];

	if (ab->alpha0 > ab->alpha1)
		AlphaDXT5_evaluatePalette8(ab, alpha_array);
	else
		AlphaDXT5_evaluatePalette6(ab, alpha_array);
	
	index_array[0x0] = ab->bits0;
	index_array[0x1] = ab->bits1;
	index_array[0x2] = ab->bits2;
	index_array[0x3] = ab->bits3;
	index_array[0x4] = ab->bits4;
	index_array[0x5] = ab->bits5;
	index_array[0x6] = ab->bits6;
	index_array[0x7] = ab->bits7;
	index_array[0x8] = ab->bits8;
	index_array[0x9] = ab->bits9;
	index_array[0xA] = ab->bitsA;
	index_array[0xB] = ab->bitsB;
	index_array[0xC] = ab->bitsC;
	index_array[0xD] = ab->bitsD;
	index_array[0xE] = ab->bitsE;
	index_array[0xF] = ab->bitsF;

    for (int i = 0; i < 16; ) {
        *dst = alpha_array[index_array[i]];
        dst += col_shift;

        i++;
        if((i&3)==0)
            dst += row_shift;
    }
}

typedef struct BlockDXT5 {
    BlockDXT5Alpha alpha;
    BlockDXT1 color;
} BlockDXT5;


static void DXT5_decode(const BlockDXT5* ab, Color32 block[16])
{
    DXT1_decode(&ab->color, block);
    AlphaDXT5_decode(&ab->alpha, block);
}

static uint8_t clamp( int x ) { return ( x<0 ? 0 : ( x>255 ? 255 : x ) ); }
static uint8_t clamp2( int x, int a, int b ) { return ( x<a ? a : ( x>b ? b : x ) ); }

static void decode_rgb( Color32 c, uint8_t* dst ) {
    int scale = ( c.b >> 3 ) + 1;
    int Co = ( (int)c.r - 128 ) / scale;
    int Cg = ( (int)c.g - 128 ) / scale;
    int Y = c.a;
    dst[0] = clamp(Y + Co - Cg);
    dst[1] = clamp(Y + Cg);
    dst[2] = clamp(Y - Co - Cg);
}

static void decode_yuv( Color32 c, uint8_t* dst[3], int offs ) {
    int scale = ( c.b >> 3 ) + 1;
    int U = ( (int)c.r - 128 ) / scale;
    int V = ( (int)c.g - 128 ) / scale;
    dst[0][offs] = clamp(c.a);//clamp2(c.a, 16, 235);
    dst[1][offs] = clamp(U + 128);//clamp2(U+128, 16, 239);
    dst[2][offs] = clamp(V + 128);//clamp2(V+128, 16, 239);
}

static const void* DXTYCoCg_decode_row(const void* src, int width, uint8_t* dst)
{
    const BlockDXT5* block = src;
    int n = width / 4;
    int pitch = width * 3;
    uint8_t* dstt;
    Color32 colors[16];

    for (int i = 0; i < n; i++) {
        DXT5_decode(block, colors);

        dstt = dst;
        for (int j = 0; j < 4; j++) {
            decode_rgb(colors[j*4+0], dstt);
            decode_rgb(colors[j*4+1], dstt+3);
            decode_rgb(colors[j*4+2], dstt+6);
            decode_rgb(colors[j*4+3], dstt+9);
            dstt += pitch;
        }
        
        dst += 3*4;
        ++block;
    }
    
    return block;
}

static const void* DXTYUV_decode_row(const void* src, int width, uint8_t* dst[3])
{
    const BlockDXT5* block = src;
    int n = width / 4;
    int pitch = width, offs;
    uint8_t* dstt[3];
    Color32 colors[16];

    dstt[0] = dst[0];
    dstt[1] = dst[1];
    dstt[2] = dst[2];
    
    for (int i = 0; i < n; i++) {
        DXT5_decode(block, colors);

        offs = 0;
        for (int j = 0; j < 4; j++) {
            decode_yuv(colors[j*4+0], dstt, 0+offs);
            decode_yuv(colors[j*4+1], dstt, 1+offs);
            decode_yuv(colors[j*4+2], dstt, 2+offs);
            decode_yuv(colors[j*4+3], dstt, 3+offs);
            offs += pitch;
        }

        dstt[0] += 4;
        dstt[1] += 4;
        dstt[2] += 4;
        ++block;
    }
    
    return block;
}

static const void* DXTYUYV_decode_row(const void* src, int width, uint8_t* dst)
{
    const BlockDXT5Alpha* block = src;
    int n = width / 8;
    int row_shift_y = width*2 - 4*2;
    int row_shift_u = width*2 - 4*4;

    for (int i = 0; i < n; i++) {
        AlphaDXT5_decode2(block+0, dst,   2, row_shift_y);  //Y0
        AlphaDXT5_decode2(block+1, dst+8, 2, row_shift_y);  //Y1
        AlphaDXT5_decode2(block+2, dst+1, 4, row_shift_u);  //U
        AlphaDXT5_decode2(block+3, dst+3, 4, row_shift_u);  //V

        dst += 8*2;
        block += 4;
    }

    return block;
}

static const void* DXTUYVY_decode_row(const void* src, int width, uint8_t* dst)
{
    const BlockDXT5Alpha* block = src;
    int n = width / 8;
    int row_shift_y = width*2 - 4*2;
    int row_shift_u = width*2 - 4*4;

    for (int i = 0; i < n; i++) {
        AlphaDXT5_decode2(block+0, dst+1, 2, row_shift_y);  //Y0
        AlphaDXT5_decode2(block+1, dst+9, 2, row_shift_y);  //Y1
        AlphaDXT5_decode2(block+2, dst,   4, row_shift_u);  //U
        AlphaDXT5_decode2(block+3, dst+2, 4, row_shift_u);  //V

        dst += 8*2;
        block += 4;
    }

    return block;
}

static const void* DXTYUYV_decode_row_planar(const void* src, int width, uint8_t* dst[3])
{
    const BlockDXT5Alpha* block = src;
    int n = width / 8;
    int row_shift = width - 4;
    uint8_t* dstt[3] = {dst[0], dst[1], dst[2]};

    for (int i = 0; i < n; i++) {
        AlphaDXT5_decode2(block+0, dstt[0], 1, row_shift);  //Y0
        AlphaDXT5_decode2(block+1, dstt[0]+4, 1, row_shift);  //Y1
        AlphaDXT5_decode2(block+2, dstt[1], 1, row_shift);  //U
        AlphaDXT5_decode2(block+3, dstt[2], 1, row_shift);  //V

        dstt[0] += 8;
        dstt[1] += 4;
        dstt[2] += 4;
        block += 4;
    }

    return block;
}

/*
*/
typedef struct YOGVideoContext {
    uint8_t * buffer;   /* block of memory for holding one frame */
    int length;         /* number of bytes in buffer */
    int awidth;         // aligned width
    int aheight;        // aligned height
    enum {
        YOG_YCoCg = 1,  // YCoCg, decoding to RGB24
        YOG_YUV,        // YUV planar, decoding to YUV444P
        YOG_YUYV,       // YUYV packed, decoding to YUYV422
    } encoding;
    uint8_t * planes[3];
    AVFrame pic;             ///< AVCodecContext.coded_frame
} YOGVideoContext;


static av_cold int yog_init_decoder(AVCodecContext *avctx)
{
    YOGVideoContext *context = avctx->priv_data;

    if(avctx->opaque)
        context->encoding = *(int*)avctx->opaque;
    else
        return -1;

    if(context->encoding == YOG_YCoCg)
        avctx->pix_fmt = PIX_FMT_RGB24;
    else if(context->encoding == YOG_YUV)
        avctx->pix_fmt = PIX_FMT_YUV444P;
    else if(context->encoding == YOG_YUYV)
        avctx->pix_fmt = PIX_FMT_YUYV422;//PIX_FMT_UYVY422;
        //avctx->pix_fmt = PIX_FMT_YUVJ422P;
    else
        return -1;
/*
    avctx->color_primaries = AVCOL_PRI_BT709;
    avctx->colorspace = AVCOL_SPC_BT709;
    avctx->color_trc = AVCOL_TRC_GAMMA22;
*/
    avctx->color_range = AVCOL_RANGE_JPEG;

    context->awidth = (avctx->width+3) & ~3;
    context->aheight = (avctx->height+3) & ~3;

    context->length = avpicture_get_size(avctx->pix_fmt, context->awidth, context->aheight);
    context->buffer = av_malloc(context->length);
    context->pic.pict_type = FF_I_TYPE;
    context->pic.key_frame = 1;

    avctx->coded_frame = &context->pic;
    
    av_log(avctx, AV_LOG_ERROR, "buffer size: %i\n", context->length);

    if (!context->buffer)
        return -1;
    
    return 0;
}


static int yog_decode(AVCodecContext *avctx,
    void *data, int *data_size,
    AVPacket *avpkt)
{
    const uint8_t *buf = avpkt->data;
    int buf_size = avpkt->size;
    YOGVideoContext *context = avctx->priv_data;
    uint8_t* dst[3];
    int nplanes = 1;
    int size;

    AVFrame * frame = (AVFrame *) data;
    AVPicture * picture = (AVPicture *) data;

    frame->interlaced_frame = avctx->coded_frame->interlaced_frame;
    frame->top_field_first = avctx->coded_frame->top_field_first;

    size = avpicture_fill(picture, context->buffer, avctx->pix_fmt, context->awidth, context->aheight);
    dst[0] = picture->data[0];
    dst[1] = picture->data[1];
    dst[2] = picture->data[2];

    if(context->encoding == YOG_YCoCg) {
        for (int i=0; i<avctx->height; i+=4) {
            buf = DXTYCoCg_decode_row(buf, context->awidth, dst[0]);
            dst[0] += picture->linesize[0];//4 * 3 * context->awidth;
        }
    }
    else if(context->encoding == YOG_YUV) {
        nplanes = 3;
        for (int i=0; i<avctx->height; i+=4) {
            buf = DXTYUV_decode_row(buf, context->awidth, dst);
            dst[0] += 4 * picture->linesize[0];//4 * context->awidth;
            dst[1] += 4 * picture->linesize[1];//4 * context->awidth;
            dst[2] += 4 * picture->linesize[2];//4 * context->awidth;
        }
    }
    else if(context->encoding == YOG_YUYV) {
        if(avctx->pix_fmt == PIX_FMT_YUYV422) {
            for (int i=0; i<avctx->height; i+=4) {
                buf = DXTYUYV_decode_row(buf, context->awidth, dst[0]);
                dst[0] += 4 * picture->linesize[0];//4 * 2 * context->awidth;
            }
        }
        else /*avctx->pix_fmt == PIX_FMT_YUVJ422P*/
        {
            for (int i=0; i<avctx->height; i+=4) {
                buf = DXTYUYV_decode_row_planar(buf, context->awidth, dst);
                dst[0] += 4 * picture->linesize[0];//4 * context->awidth;
                dst[1] += 2 * picture->linesize[1];//2 * context->awidth;
                dst[2] += 2 * picture->linesize[2];//2 * context->awidth;
            }
        }
    }

    for (int i=0; i<nplanes; ++i) {
        picture->data[i] += picture->linesize[i] * (context->aheight-1);
        picture->linesize[i] = -picture->linesize[i];
    }

    *data_size = sizeof(AVPicture);
    return buf_size;
}

static av_cold int yog_close_decoder(AVCodecContext *avctx)
{
    YOGVideoContext *context = avctx->priv_data;

    av_freep(&context->buffer);
    return 0;
}

AVCodec yog_decoder = {
    "yogvideo",
    CODEC_TYPE_VIDEO,
    CODEC_ID_YOG,
    sizeof(YOGVideoContext),
    yog_init_decoder,
    NULL,
    yog_close_decoder,
    yog_decode,
    .long_name = NULL_IF_CONFIG_SMALL("YOG video"),
};
