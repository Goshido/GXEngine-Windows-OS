//version 1.2

#ifndef GX_FONT_EXT
#define GX_FONT_EXT


#include "GXFont.h"
#include "GXOpenGL.h"
#include <GXCommon/GXMath.h>


struct GXGlyph
{
	GXVec2		min;
	GXVec2		max;
	GXFloat		offsetY;
	GXUShort	advance;
	GXByte		atlasID;
};

struct GXGlyphInfo
{
	GLuint			atlas;
	GXVec2			min;
	GXVec2			max;
	GXFloat			offsetY;
	GXFloat			width;
	GXFloat			height;
	GXUShort		advance;
};

class GXFontExt
{
	private:
		FT_Face		face;
		GXUShort	size;
		GXGlyph		glyphs[ 0x7FFF ];
		GLuint*		atlases;
		GXByte		atlasID;
		GXUShort	left;
		GXUShort	top;
		GXUShort	bottom;
		GXUShort	spaceAdvance;

	public:
		GXFontExt ( FT_Face face, GXUShort size );
		~GXFontExt ();

		GXBool GetGlyph ( GXUInt symbol, GXGlyphInfo &info );
		GXInt GetKerning ( GXUInt symbol, GXUInt prevSymbol );
		GXUShort GetSpaceAdvance ();
		GXUShort GetSize ();

	private:
		GXVoid RenderGlyph ( GXUInt symbol );
		GXVoid CreateAtlas ();
		GXUByte CheckAtlas ( GXUInt width, GXUInt height );
};


#endif //GX_FONT_EXT
