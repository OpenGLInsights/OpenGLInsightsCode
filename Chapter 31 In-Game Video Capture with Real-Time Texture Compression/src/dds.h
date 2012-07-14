#ifndef __ENG3_RENDERER_DDS_H__
#define __ENG3_RENDERER_DDS_H__

#include <base/pixelfmt.h>

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

namespace dds {

void* get_texture_data(const char* f, int* w, int* h, int* mips, base::pixelfmt* pixmft);

bool save_texture(const char* file, int w, int h, int mips, base::pixelfmt fmt, const void* data);

} //namespace dds

#endif // __ENG3_RENDERER_DDS_H__