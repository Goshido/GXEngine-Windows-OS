//version 1.3

#ifndef GX_FONT_STORAGE
#define GX_FONT_STORAGE


#include "GXFont.h"


FT_Face GXCALL GXGetFont ( const GXWChar* fileName );
GXVoid GXCALL GXRemoveFont ( FT_Face font );

GXVoid GXCALL GXCleanFonts ();
GXUInt GXCALL GXGetTotalFontStorageObjects ( GXWChar** lastObject );


#endif //GX_FONT_STORAGE