//versoin 1.5


#ifndef GX_FONT_RENDERER
#define GX_FONT_RENDERER


#include "GXTextureUtils.h"
#include "GXFontStorage.h"
#include <GXCommon/GXMath.h>


struct GXFontInfo
{
	GXGLTextureStruct*	texture;
	GLuint				textureID;

	FT_Face				font;
	GXVec4*				fontColor;
	GXUInt				fontSize;
	GXFloat				fontAspect;
	GXBool				kerning;

	GXInt				insertX;
	GXInt				insertY;
};


class GXFontRenderer
{
	protected:
		GXVoid*		data;
		GXUShort	width;
		GXUShort	height;

	public:
		GXFontRenderer ();
		~GXFontRenderer ();

		GXVoid Reset ( const GXFontInfo &fontInfo );
		GXVoid AddImage ( const GXGLTextureStruct &texture, GLuint textureID );
		GXInt AddText ( const GXFontInfo &fontInfo, const GXWChar* fmt, va_list op );
		GXVoid Render ( const GXFontInfo &fontInfo );

		GXUInt GetTextRasterLength ( const FT_Face font, GXUInt fontSize, GXBool kerning, const GXWChar* fmt, va_list op );

	private:
		GXVoid DumpGlyphToFile ( const GXWChar* fileName, const FT_Bitmap &bitmap );
};


#endif //GX_FONT_RENDERER