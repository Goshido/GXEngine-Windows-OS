//version 1.4

#ifndef GX_FONT_STORAGE_EXT
#define GX_FONT_STORAGE_EXT


#include "GXFontExt.h"


GXFontExt* GXCALL GXGetFontExt ( const GXWChar* fileName, GXUShort size );
GXVoid GXCALL GXRemoveFontExt ( const GXFontExt* font );
GXUInt GXCALL GXGetTotalFontStorageObjectsExt ( const GXWChar** lastFont );


#endif //GX_FONT_STORAGE_EXT
