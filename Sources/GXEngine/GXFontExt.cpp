//version 1.2

#include <GXEngine/GXFontExt.h>
#include <GXEngine/GXTextureUtils.h>
#include <GXCommon/GXLogger.h>


#define GX_ATLAS_UNDEFINED		-1
#define GX_ATLAS_NEW_LINE		0
#define GX_ATLAS_NEW_TEXTURE	1
#define GX_ATLAS_FILL			2
#define GX_ATLAS_RESOLUTION		512
#define GX_ATLAS_ONE_PIXEL		0.0019531f	//1 pixel in uv coordinate system
#define GX_ATLAS_SPACING		1


GXFontExt::GXFontExt ( FT_Face face, GXUShort size )
{
	this->face = face;
	this->size = size;

	left = bottom = top = 0;

	for ( GXUShort i = 0; i < 0x7FFF; i++ )
		glyphs[ i ].atlasID = GX_ATLAS_UNDEFINED;

	atlases = 0;
	atlasID = GX_ATLAS_UNDEFINED;

	GXUShort temp =  (GXUShort)( size * 0.5f );
	spaceAdvance = temp == 0 ? 1 : temp;
}

GXFontExt::~GXFontExt ()
{
	if ( atlasID == GX_ATLAS_UNDEFINED )
		return;

	glDeleteTextures ( atlasID + 1, atlases );

	free ( atlases );
}

GXBool GXFontExt::GetGlyph ( GXUInt symbol, GXGlyphInfo &info )
{
	if ( symbol > 0x7FFF )
		return GX_FALSE;

	if ( glyphs[ symbol ].atlasID != GX_ATLAS_UNDEFINED )
	{
		info.atlas = atlases[ glyphs[ symbol ].atlasID ];
		memcpy ( &info.min, &glyphs[ symbol ].min, sizeof ( GXVec2 ) );
		memcpy ( &info.max, &glyphs[ symbol ].max, sizeof ( GXVec2 ) );
		info.offsetY = glyphs[ symbol ].offsetY;
		info.width = ( glyphs[ symbol ].max.x - glyphs[ symbol ].min.x ) * GX_ATLAS_RESOLUTION;
		info.height = ( glyphs[ symbol ].max.y - glyphs[ symbol ].min.y ) * GX_ATLAS_RESOLUTION;
		info.advance = glyphs[ symbol ].advance;

		return GX_TRUE;
	}

	RenderGlyph ( symbol );

	info.atlas = atlases[ glyphs[ symbol ].atlasID ];
	memcpy ( &info.min, &glyphs[ symbol ].min, sizeof ( GXVec2 ) );
	memcpy ( &info.max, &glyphs[ symbol ].max, sizeof ( GXVec2 ) );
	info.offsetY = glyphs[ symbol ].offsetY;
	info.width = ( glyphs[ symbol ].max.x - glyphs[ symbol ].min.x ) * GX_ATLAS_RESOLUTION;
	info.height = ( glyphs[ symbol ].max.y - glyphs[ symbol ].min.y ) * GX_ATLAS_RESOLUTION;
	info.advance = glyphs[ symbol ].advance;

	return GX_TRUE;
}

GXInt GXFontExt::GetKerning ( GXUInt symbol, GXUInt prevSymbol )
{
	FT_UInt glyphIndex = GXFtGetCharIndex ( face, symbol );
	FT_UInt prevGlyphIndex = GXFtGetCharIndex ( face, prevSymbol );

	if ( FT_HAS_KERNING ( face ) )
	{
		FT_Vector delta;
		GXFtGetKerning ( face, prevGlyphIndex, glyphIndex, FT_KERNING_DEFAULT, &delta );
		return (GXInt)( delta.x >> 6 );
	}

	return 0;
}

GXUShort GXFontExt::GetSpaceAdvance ()
{
	return spaceAdvance;
}

GXUShort GXFontExt::GetSize ()
{
	return size;
}

GXVoid GXFontExt::RenderGlyph ( GXUInt symbol )
{
	if ( GXFtSetPixelSizes ( face, size, size ) )
		GXLogA ( "GXFont::RenderGlyph::Error - Can't set font size %i\n", size );

	FT_UInt glyphIndex = GXFtGetCharIndex ( face, symbol );

	if ( GXFtLoadGlyph ( face, glyphIndex, FT_LOAD_RENDER ) )
		GXLogA ( "GXFont::RenderGlyph::Error - FT_Load_Glyph failed at symbol #%i\n", symbol );

	if ( GXFtRenderGlyph ( face->glyph, FT_RENDER_MODE_NORMAL ) )
		GXLogA ( "GXFont::RenderGlyph::Error - FT_Render_Glyph failed at symbol #%i\n", symbol );

	glyphs[ symbol ].offsetY = (GXFloat)( ( face->glyph->metrics.horiBearingY - face->glyph->metrics.height ) >> 6 );
	glyphs[ symbol ].advance = (GXUShort)( face->glyph->advance.x >> 6 );

	FT_Bitmap bitmap = face->glyph->bitmap;

	switch ( CheckAtlas ( bitmap.width, bitmap.rows ) )
	{
		case GX_ATLAS_NEW_LINE:
			left = 0;
			bottom = top;
			top = bottom;
		break;

		case GX_ATLAS_NEW_TEXTURE:
			CreateAtlas ();

			left = 0;
			bottom = 0;
			top = 0;
		break;
	}

	glyphs[ symbol ].min = GXCreateVec2 ( left * GX_ATLAS_ONE_PIXEL, ( bottom ) * GX_ATLAS_ONE_PIXEL );
	glyphs[ symbol ].max = GXCreateVec2 ( ( left + bitmap.width ) * GX_ATLAS_ONE_PIXEL, ( bottom + bitmap.rows ) * GX_ATLAS_ONE_PIXEL );

	glyphs[ symbol ].atlasID = atlasID;

	GXUByte* buffer = (GXUByte*)malloc ( bitmap.width * bitmap.rows );

	for ( GXUInt h = 0; h < (GXUInt)bitmap.rows; h ++ )
		for ( GXUInt w = 0; w < (GXUInt)bitmap.width; w ++ )
			buffer[ h * bitmap.width + w ] = bitmap.buffer[ ( bitmap.rows - 1 - h ) * bitmap.width + w ];

	glBindTexture ( GL_TEXTURE_2D, atlases[ atlasID ] );
	glPixelStorei ( GL_UNPACK_ALIGNMENT, 1 );
	glTexSubImage2D ( GL_TEXTURE_2D, 0, left, bottom, bitmap.width, bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, buffer );
	glBindTexture ( GL_TEXTURE_2D, 0 );

	free ( buffer );

	left += bitmap.width + GX_ATLAS_SPACING;

	if ( top < ( bottom + bitmap.rows + GX_ATLAS_SPACING ) )
		top = bottom + bitmap.rows + GX_ATLAS_SPACING;
}

GXVoid GXFontExt::CreateAtlas ()
{
	if ( atlasID == GX_ATLAS_UNDEFINED )
	{
		atlasID = 0;
		atlases = (GLuint*)malloc ( sizeof ( GLuint ) );
	}
	else
	{
		GLuint* temp = (GLuint*)malloc ( ( atlasID + 2 ) * sizeof ( GLuint ) );
		for ( GXUShort i = 0; i < atlasID; i++ )
			temp[ i ] = atlases[ i ];

		atlasID++;
		free ( atlases );
		atlases = temp;
	}

	GXUInt size = GX_ATLAS_RESOLUTION * GX_ATLAS_RESOLUTION;
	GXUByte* data = (GXUByte*)malloc ( size );

	memset ( data, 0, size );

	GXGLTextureStruct ts;
	ts.format = GL_RED;
	ts.internalFormat = GL_R8;
	ts.type = GL_UNSIGNED_BYTE;
	ts.width = GX_ATLAS_RESOLUTION;
	ts.height = GX_ATLAS_RESOLUTION;
	ts.wrap = GL_CLAMP_TO_EDGE;
	ts.anisotropy = 1;
	ts.resampling = GX_TEXTURE_RESAMPLING_NONE;
	ts.data = data;

	atlases[ atlasID ] = GXCreateTexture ( ts );

	free ( data );
}

GXUByte GXFontExt::CheckAtlas ( GXUInt width, GXUInt height )
{
	if ( atlasID == GX_ATLAS_UNDEFINED )
		return GX_ATLAS_NEW_TEXTURE;

	if ( ( left + width ) >= GX_ATLAS_RESOLUTION )
	{
		if ( ( bottom + height ) >= GX_ATLAS_RESOLUTION )
			return GX_ATLAS_NEW_TEXTURE;
		else
			return GX_ATLAS_NEW_LINE;
	}

	if ( ( bottom + height ) >= GX_ATLAS_RESOLUTION )
		return GX_ATLAS_NEW_TEXTURE;

	return GX_ATLAS_FILL;
}
