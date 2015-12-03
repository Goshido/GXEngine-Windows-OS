//version 1.7

#ifndef GX_TEXTURE_STORAGE_EXT
#define GX_TEXTURE_STORAGE_EXT


#include "GXOpenGL.h"
#include <GXCommon/GXMTRStructs.h>


struct GXTexture
{
	GXUInt		width;
	GXUInt		height;
	GXUByte		channels;
	GLuint		texObj;
};


GXVoid GXCALL GXLoadTexture ( const GXWChar* fileName, GXTexture &texture );

GXVoid GXCALL GXGetTextureExt ( GXTextureInfo &tex_info );
GXVoid GXCALL GXAddRefTextureExt ( const GXTexture &texture );

GXVoid GXCALL GXRemoveTextureExt ( const GXTexture &texture );
GXVoid GXCALL GXRemoveTextureExt ( GXTextureInfo &tex_info );

GXUInt GXCALL GXGetTotalTextureStorageObjectsExt ( const GXWChar** lastTexture );


#endif // GX_TEXTURE_STORAGE_EXT
