//version 1.4

#ifndef GX_TEXTURE_STORAGE
#define GX_TEXTURE_STORAGE


#include "GXOpenGL.h"
#include <GXCommon/GXMTRStructs.h>


GLuint GXCALL GXGetTexture ( GXTextureInfo &tex_info );
GXVoid GXCALL GXRemoveTexture ( GXTextureInfo &tex_info );
GXUInt GXCALL GXGetTotalTextureStorageObjects ( GXWChar** lastTexture );


#endif // GX_TEXTURE_STORAGE