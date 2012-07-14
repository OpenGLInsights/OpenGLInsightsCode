
/**********************************************************************\
* AUTHOR : HILLAIRE Sébastien
*
* MAIL   : hillaire_sebastien@yahoo.fr
* SITE   : sebastien.hillaire.free.fr
*
*	You are free to totally or partially use this file/code.
* If you do, please credit me in your software or demo and leave this
* note.
*	Share your work and your ideas as much as possible!
\*********************************************************************/

#include "LoadTexturesFunctions.h"




#define BMP_DWORD			unsigned long
#define BMP_BYTE			unsigned char
#define BMP_WORD			unsigned short
struct BMPHeader
{
	BMP_BYTE		bfType1;
	BMP_BYTE		bfType2;
	BMP_DWORD		bfSize;
	BMP_WORD		bfReserved1;
	BMP_WORD		bfReserved2;
	BMP_DWORD		bfOffBits;
	BMP_DWORD		biSize;
	BMP_DWORD		biWidth;
	BMP_DWORD		biHeight;
	BMP_WORD		biPlanes;
	BMP_WORD		biBitCount;
	BMP_DWORD		biCompression;
	BMP_DWORD		biSizeImage;
	BMP_DWORD		biXPelsPerMeter;
	BMP_DWORD		biYPelsPerMeter;
	BMP_DWORD		biClrUsed;
	BMP_DWORD		biClrImportant;
};



bool loadTexture(const char* filepath, GLuint* texture)
{
	BMPHeader bmph;

	FILE* f = fopen(filepath,"rb");
	if(f!=NULL)
	{
		//read header
		fread(&bmph.bfType1,1,sizeof(BMP_BYTE),f);
		fread(&bmph.bfType2,1,sizeof(BMP_BYTE),f);
		fread(&bmph.bfSize,1,sizeof(BMP_DWORD),f);
		fread(&bmph.bfReserved1,1,sizeof(BMP_WORD),f);
		fread(&bmph.bfReserved2,1,sizeof(BMP_WORD),f);
		fread(&bmph.bfOffBits,1,sizeof(BMP_DWORD),f);
		if (bmph.bfOffBits==54)//we only read this type of bmp so becareful
		{
			fread(&bmph.biSize,1,sizeof(BMP_DWORD),f);
			fread(&bmph.biWidth,1,sizeof(BMP_DWORD),f);
			fread(&bmph.biHeight,1,sizeof(BMP_DWORD),f);
			fread(&bmph.biPlanes,1,sizeof(BMP_WORD),f);
			fread(&bmph.biBitCount,1,sizeof(BMP_WORD),f);
			fread(&bmph.biCompression,1,sizeof(BMP_DWORD),f);
			fread(&bmph.biSizeImage,1,sizeof(BMP_DWORD),f);
			fread(&bmph.biXPelsPerMeter,1,sizeof(BMP_DWORD),f);
			fread(&bmph.biYPelsPerMeter,1,sizeof(BMP_DWORD),f);
			fread(&bmph.biClrUsed,1,sizeof(BMP_DWORD),f);
			fread(&bmph.biClrImportant,1,sizeof(BMP_DWORD),f);

			if(bmph.biBitCount==24)
			{
				unsigned char* data = new unsigned char[bmph.biWidth*bmph.biHeight*3];

				//load image's datas in BGR order
				fread(&data[0], sizeof(unsigned char), bmph.biWidth*bmph.biHeight*3, f);

				glGenTextures(1,texture);
				glBindTexture(GL_TEXTURE_2D,*texture);
				glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
				glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
				glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
				//glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR );
				//glTexImage2D(GL_TEXTURE_2D,0,GL_RGB8,bmph.biWidth,bmph.biHeight,0,GL_BGR,GL_UNSIGNED_BYTE,data);
				glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR );
				gluBuild2DMipmaps(GL_TEXTURE_2D,GL_RGB8,bmph.biWidth,bmph.biHeight,GL_BGR,GL_UNSIGNED_BYTE,data);
				glBindTexture(GL_TEXTURE_2D,0);

				delete data;
			}
			else
				return false;
		}

		fclose(f);
		return true;
	}
	return false;
}
void unloadTexture(GLuint* texture)
{
	if( texture!=NULL && (*texture)!=0 )
		glDeleteTextures(1,texture);
}

