/*
 * Main program to create a distance field from a
 * TGA file, and write the result to a TGA file.
 * Public domain code, originally by Nicolas Rougier
 * (nicolas.rougier@gmail.com), edited by Stefan
 * Gustavson (stefan.gustavson@gmail.com) to output
 * 16-bit distance data for better range and accuracy.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "edtaa3func.c"

// Define some fixed size types.
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;

// TGA Header struct to make it simple to dump a TGA to disk.
#if defined(_MSC_VER) || defined(__GNUC__)
#pragma pack(push, 1)
#pragma pack(1)               // Dont pad the following struct.
#endif
typedef struct _TGA_header
{
  uint8   idLength,           // Length of optional identification sequence.
          paletteType,        // Is a palette present? (1=yes)
          imageType;          // Image data type (0=none, 1=indexed, 2=rgb,
                              // 3=grey, +8=rle packed).
  uint16  firstPaletteEntry,  // First palette index, if present.
          numPaletteEntries;  // Number of palette entries, if present.
  uint8   paletteBits;        // Number of bits per palette entry.
  uint16  x,                  // Horiz. pixel coord. of lower left of image.
          y,                  // Vert. pixel coord. of lower left of image.
          width,              // Image width in pixels.
          height;             // Image height in pixels.
  uint8   depth,              // Image color depth (bits per pixel).
          descriptor;         // Image attribute flags.
} TGA_header;
#if defined(_MSC_VER) || defined(__GNUC__)
#pragma pack(pop)
#endif

int TGA_write( char * filename,
           unsigned int width,
           unsigned int height,
           unsigned char *data )
{
    TGA_header header;
    memset( &header, 0, sizeof(TGA_header) );
    header.imageType  = 3;
    header.width = width;
    header.height = height;
    header.depth = 8;
    header.descriptor = 0x20; // Flip u/d, no alpha bits

    FILE *file;
    file = fopen(filename, "wb");

    if( !file )
    {
        fprintf(stderr, "Failed to write file.\n");
        return 0;
    }

    fwrite( (const char *) &header, sizeof(TGA_header), 1, file );
    fwrite( data, sizeof(unsigned char), width * height, file);
    fclose(file);

    return 1;
}

int TGA_writeRGB( char * filename,
           unsigned int width,
           unsigned int height,
           unsigned char *data )
{
    TGA_header header;
    memset( &header, 0, sizeof(TGA_header) );
    header.imageType  = 2;
    header.width = width;
    header.height = height;
    header.depth = 24; // RGB, 8 bits each
    header.descriptor = 0x20; // Flip u/d, no alpha bits

    FILE *file;
    file = fopen(filename, "wb");

    if( !file )
    {
        fprintf(stderr, "Failed to write file.\n");
        return 0;
    }

    fwrite( (const char *) &header, sizeof(TGA_header), 1, file );
    fwrite( data, sizeof(unsigned char), 3 * width * height, file);
    fclose(file);

    return 1;
}

unsigned char *TGA_read( char *filename,
                         unsigned int * width,
                         unsigned int * height,
                         unsigned int * depth )
{
    FILE *file;
    unsigned char type[4];
    unsigned char info[6];

    file = fopen( filename, "rb" );
    if( !file )
    {
        return 0;
    }
    fread( &type, sizeof (char), 3, file );
    fseek( file, 12, SEEK_SET );
    fread( &info, sizeof (char), 6, file );

    /* Allow unpaletted images type 2 (color) or 3 (greyscale) */
    if( type[1] != 0 || (type[2] != 2 && type[2] != 3) )
    {
        fclose( file );
        return 0;
    }

    *width  = info[0] + info[1] * 256;
    *height = info[2] + info[3] * 256;
    *depth  = info[4] / 8;

    if( *depth != 1 && *depth != 3 && *depth != 4 )
    {
        fclose(file);
        return 0;
    }

    // Allocate memory for image data
    unsigned long size =  *height * *width * *depth;
    unsigned char *data = (unsigned char *) malloc( size * sizeof(unsigned char) );

    // Read in image data
    fread( data, sizeof(unsigned char), size, file );
    fclose( file );

    return data;
}

unsigned char *
make_distance_map( unsigned char *img, unsigned int width, unsigned int height )
{
    short * xdist = (short *)  malloc( width * height * sizeof(short) );
    short * ydist = (short *)  malloc( width * height * sizeof(short) );
    double * gx   = (double *) calloc( width * height, sizeof(double) );
    double * gy      = (double *) calloc( width * height, sizeof(double) );
    double * data    = (double *) calloc( width * height, sizeof(double) );
    double * outside = (double *) calloc( width * height, sizeof(double) );
    double * inside  = (double *) calloc( width * height, sizeof(double) );
    int i;

    // Convert img into double (data)
    double img_min = 255, img_max = -255;
    for( i=0; i<width*height; i++)
    {
        double v = img[i];
        data[i] = v;
        if (v > img_max) img_max = v;
        if (v < img_min) img_min = v;
    }
    // Rescale image levels between 0 and 1
    double img_offset = (img_min < 128.0) ? img_min : 0;
    double img_range = img_max-img_min;
    if(img_range == 0.0) img_range = 255.0; // Failsafe for constant image
    for( i=0; i<width*height; i++)
    {
        data[i] = (img[i]-img_offset)/(img_range);
    }

    // Transform background (outside contour, in areas of 0's)
    computegradient( data, height, width, gx, gy);
    edtaa3(data, gx, gy, height, width, xdist, ydist, outside);
    for( i=0; i<width*height; i++)
        if( outside[i] < 0.0 )
            outside[i] = 0.0;

    // Transform foreground (inside contour, in areas of 1's)
    memset(gx, 0, sizeof(double)*width*height );
    memset(gy, 0, sizeof(double)*width*height );
    for( i=0; i<width*height; i++)
        data[i] = 1 - data[i];
    computegradient( data, height, width, gx, gy);
    edtaa3(data, gx, gy, height, width, xdist, ydist, inside);
    for( i=0; i<width*height; i++)
        if( inside[i] < 0.0 )
            inside[i] = 0.0;

    // The bipolar distance field is now outside-inside

/* Single channel 8-bit output (bad precision and range, but simple) */
/*
    unsigned char *out = (unsigned char *) malloc( width * height * sizeof(unsigned char) );
    for( i=0; i<width*height; i++)
    {
        outside[i] -= inside[i];
        outside[i] = 128+outside[i]*16;
        if( outside[i] < 0 ) outside[i] = 0;
        if( outside[i] > 255 ) outside[i] = 255;
        out[i] = 255 - (unsigned char) outside[i];
    }
*/

/* Dual channel 16-bit output (more complicated, but good precision and range) */
    unsigned char *out = (unsigned char *) malloc( width * height * 3 * sizeof(unsigned char) );
    double dist;
    for( i=0; i<width*height; i++)
    {
        dist = outside[i] - inside[i];
        dist = 128.0 + dist;
        if( dist < 0.0 ) dist = 0.0;
        if( dist >= 256.0 ) dist = 255.999;
        out[3*i + 2] = (unsigned char) dist; // R channel is truncated integer part
        out[3*i + 1] = (unsigned char) ((dist - floor(dist)) * 256.0); // G channel is fraction
        out[3*i] = img[i]; // B channel is a copy of the original grayscale image
    }

    free( xdist );
    free( ydist );
    free( gx );
    free( gy );
    free( data );
    free( outside );
    free( inside );
    return out;
}

int main( int argc, char **argv )
{
    unsigned int width=0, height=0, depth=0;
    if(argc !=3)
    {
      fprintf( stderr, "Usage: makedist infile.tga outfile.tga\n");
      return EXIT_FAILURE;
    }
    unsigned char *img = TGA_read(argv[1], &width, &height, &depth);
    if (!img)
    {
      fprintf( stderr, "Failed to read image '%s' !\n", argv[1] );
      return EXIT_FAILURE;
    }
    printf("Input image: %d x %d pixels, %d channels\n", width, height, depth);
    if (depth != 1)
    {
      fprintf( stderr, "Input image '%s' has to be grayscale !\n", argv[1] );
      free(img);
      return EXIT_FAILURE;
    }

    unsigned char *out = make_distance_map(img, width, height);
    // TGA_write(argv[2], width, height, out); // 8 bit grayscale
    TGA_writeRGB(argv[2], width, height, out); // 24 bit RGB

    free(img);
    free(out);
    return EXIT_SUCCESS;
}
