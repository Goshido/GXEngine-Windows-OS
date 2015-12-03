//version 1.2

#ifndef GX_FONT
#define GX_FONT


#include <GXEngineDLL/GXEngineAPI.h>


extern FT_Library				gx_ft_Library;

//-------------------------------------------------------------------------

extern PFNFTNEWMEMORYFACE		GXFtNewMemoryFace;
extern PFNFTDONEFACE			GXFtDoneFace;

extern PFNFTSETCHARSIZE			GXFtSetCharSize;
extern PFNFTSETPIXELSIZES		GXFtSetPixelSizes;

extern PFNFTGETCHARINDEX		GXFtGetCharIndex;
extern PFNFTGETKERNING			GXFtGetKerning;

extern PFNFTLOADGLYPH			GXFtLoadGlyph;
extern PFNFTRENDERGLYPH			GXFtRenderGlyph;

//-------------------------------------------------------------------------

GXBool GXCALL GXFontInit ();
GXBool GXCALL GXFontDestroy ();


#endif //GX_FONT