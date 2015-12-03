//version 1.2

#include <GXEngine/GXFont.h>


PFNFTNEWMEMORYFACE		GXFtNewMemoryFace			= 0;
PFNFTDONEFACE			GXFtDoneFace				= 0;

PFNFTSETCHARSIZE		GXFtSetCharSize				= 0;
PFNFTSETPIXELSIZES		GXFtSetPixelSizes			= 0;

PFNFTGETCHARINDEX		GXFtGetCharIndex			= 0;
PFNFTGETKERNING			GXFtGetKerning				= 0;

PFNFTLOADGLYPH			GXFtLoadGlyph				= 0;
PFNFTRENDERGLYPH		GXFtRenderGlyph				= 0;

PFNGXFREETYPEDESTROY	GXFreeTypeDestroy			= 0;

//-------------------------------------------------------------------------

extern HMODULE gx_GXEngineDLLModuleHandle;

FT_Library	gx_ft_Library;


GXBool GXCALL GXFontInit ()
{
	gx_GXEngineDLLModuleHandle = LoadLibraryW ( L"GXEngine.dll" );
	if ( !gx_GXEngineDLLModuleHandle )
	{
		GXLogW ( L"GXFontInit::Error - Не удалось загрузить GXEngine.dll" );
		return GX_FALSE;
	}

	PFNGXFREETYPEINIT GXFreeTypeInit;
	GXFreeTypeInit = ( PFNGXFREETYPEINIT )GetProcAddress ( gx_GXEngineDLLModuleHandle, "?GXFreeTypeInit@@YG_NAAUGXFreeTypeFunctions@@AAPAUFT_LibraryRec_@@@Z" );
	if ( !GXFreeTypeInit )
	{
		GXLogW ( L"GXFontInit::Error - Не удалось найти функцию GXFreeTypeInit" );
		return GX_FALSE;
	}

	GXFreeTypeFunctions out;
	out.FT_Done_Face = &GXFtDoneFace;
	out.FT_Get_Char_Index = &GXFtGetCharIndex;
	out.FT_Get_Kerning = &GXFtGetKerning;
	out.FT_Load_Glyph = &GXFtLoadGlyph;
	out.FT_New_Memory_Face = &GXFtNewMemoryFace;
	out.FT_Render_Glyph = &GXFtRenderGlyph;
	out.FT_Set_Char_Size = &GXFtSetCharSize;
	out.FT_Set_Pixel_Sizes = &GXFtSetPixelSizes;
	out.GXFreeTypeDestroy = &GXFreeTypeDestroy;

	return GXFreeTypeInit ( out, gx_ft_Library );
}

GXBool GXCALL GXFontDestroy ()
{
	if ( !gx_GXEngineDLLModuleHandle )
	{
		GXLogW ( L"GXFontDestroy::Error - Попытка выгрузить несуществующую в памяти GXEngine.dll" );
		return GX_FALSE;
	}

	GXFreeTypeDestroy ();

	if ( !FreeLibrary ( gx_GXEngineDLLModuleHandle ) )
	{
		GXLogW ( L"GXFontDestroy::Error - Не удалось выгрузить библиотеку GXEngine.dll" );
		return GX_FALSE;
	}

	return GX_TRUE;
}