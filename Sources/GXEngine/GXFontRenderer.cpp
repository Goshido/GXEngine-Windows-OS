//version 1.5

#include <GXEngine/GXFontRenderer.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXFileSystem.h>


GXFontRenderer::GXFontRenderer ()
{
	data = 0;
	width = 0;
	height = 0;
}

GXFontRenderer::~GXFontRenderer ()
{
	GXSafeFree ( data );
}

GXVoid GXFontRenderer::Reset ( const GXFontInfo &fontInfo )
{
	GXInt size = fontInfo.texture->width * fontInfo.texture->height * 4;	//RGBA x 1 байт

	if ( fontInfo.texture->width != width || fontInfo.texture->height != height )
	{
		GXSafeFree ( data )

		width = fontInfo.texture->width;
		height = fontInfo.texture->height;

		data = malloc ( size );
	}

	memset ( data, 0, size );
}

GXVoid GXFontRenderer::AddImage ( const GXGLTextureStruct &texture, GLuint textureID )
{
	if ( texture.internalFormat != GL_RGBA8 )
	{
		GXLogW ( L"GXFontRenderer::AddTexture::Error - Поддерживаются только текстуры GL_RGBA8\n" );
		return;
	}

	GXUInt size = texture.width * texture.height * 4;	//RGBA x 1 байт

	if ( texture.width != width || texture.height != height )
	{
		GXSafeFree ( data )

		width = texture.width;
		height = texture.height;

		data = malloc ( size );
	}

	glBindTexture ( GL_TEXTURE_2D, textureID );
	glGetTexImage ( GL_TEXTURE_2D, 0, texture.format, texture.type, data );
	glBindTexture ( GL_TEXTURE_2D, 0 );
}

GXInt GXFontRenderer::AddText ( const GXFontInfo &fontInfo, const GXWChar* fmt, va_list op )
{
	GXUInt total = ( width * height ) << 2; //RGBA x 1 байт

	if ( fontInfo.texture->width != width || fontInfo.texture->height != height )
	{
		GXSafeFree ( data )

		width = fontInfo.texture->width;
		height = fontInfo.texture->height;	

		data = malloc ( total );	
		memset ( data, 0, total );
	}

	//if ( GXFtSetCharSize ( face, fontInfo.fontSize << 6, fontInfo.fontSize << 6, 96, 96 ) )
	if ( GXFtSetPixelSizes ( fontInfo.font, fontInfo.fontSize, fontInfo.fontSize ) )
		GXLogW ( L"GXFontRenderer::AddText::Error - Не удалось выставить размер шрифта %i для текста с форматирующей строкой [%s]\n", fontInfo.fontSize, fmt );

	FT_UInt glyphIndex;
	FT_UInt glyphIndexPrev = 0;

	FT_Bool supportKerning = FT_HAS_KERNING ( fontInfo.font );

	GXWChar buffer[ 1024 ];
	vswprintf ( buffer, 1023, fmt, op );

	GXUInt len = GXWcslen ( buffer );

	GXInt penX = fontInfo.insertX;
	GXInt penY = fontInfo.insertY;

	for ( GXUInt i = 0; i < len; i++ )
	{
		glyphIndex = GXFtGetCharIndex ( fontInfo.font, buffer[ i ] );

		if ( supportKerning && glyphIndexPrev && fontInfo.kerning )
		{
			FT_Vector delta;

			GXFtGetKerning ( fontInfo.font, glyphIndexPrev, glyphIndex, FT_KERNING_DEFAULT, &delta );

			penX += delta.x >> 6;
		}

		if ( GXFtLoadGlyph ( fontInfo.font, glyphIndex, FT_LOAD_RENDER ) )
			GXLogW ( L"GXFontRenderer::AddText::Error - FT_Load_Glyph провалился в строке [%s] на символе #%i (%c)\n", buffer, i, buffer[ i ] );

		if ( GXFtRenderGlyph ( fontInfo.font->glyph, FT_RENDER_MODE_NORMAL ) )
			GXLogW ( L"GXFontRenderer::AddText::Error - FT_Render_Glyph провалился в строке [%s] на символе #%i (%c)\n", buffer, i, buffer[ i ] );

		FT_Bitmap bitmap = fontInfo.font->glyph->bitmap;

		GXUChar* image = (GXUChar*)data;
				
		//Отображение в большой битмап
		GXInt x = penX;
		GXInt y = penY;

		GXInt deltaY = ( fontInfo.font->glyph->metrics.height >> 6 ) - ( fontInfo.font->glyph->metrics.horiBearingY >> 6 );

		for ( GXInt h = 0; h < bitmap.rows; h++ )
		{
			for ( GXInt w = 0; w < bitmap.width; w++ )
			{
				GXInt factorY = y - deltaY;
				if ( factorY < 0 || factorY >= fontInfo.texture->height ) continue;

				GXInt factorX = x + w;
				if ( factorX < 0 || factorX >= fontInfo.texture->width ) continue;

				GXUInt pos = ( factorY * fontInfo.texture->width + factorX ) << 2;
				
				#define UCHAR_2_FLOAT 0.0039216f		// 1.0f / 255.0f
				#define FLOAT_2_UCHAR 255.0f

				GXUChar sample = bitmap.buffer[ ( bitmap.rows - 1 - h ) * bitmap.width + w ];
				GXFloat srcAlpha = image[ pos + 3 ] * UCHAR_2_FLOAT;
				GXFloat blend = sample * UCHAR_2_FLOAT * fontInfo.fontColor->a;
				GXFloat invBlend = 1.0f - blend;
	
				image[ pos ] = (GXUChar)( ( image[ pos ] * UCHAR_2_FLOAT * invBlend + fontInfo.fontColor->r * blend ) * FLOAT_2_UCHAR );	//R
				pos++;
				image[ pos ] = (GXUChar)( ( image[ pos ] * UCHAR_2_FLOAT * invBlend + fontInfo.fontColor->g * blend ) * FLOAT_2_UCHAR );	//G
				pos++;
				image[ pos ] = (GXUChar)( ( image[ pos ] * UCHAR_2_FLOAT * invBlend + fontInfo.fontColor->b * blend ) * FLOAT_2_UCHAR );	//B
				pos++;
				image[ pos ] = (GXUChar)( ( srcAlpha + ( 1.0 - srcAlpha ) * blend ) *  FLOAT_2_UCHAR );	//A
				pos++;

				#undef FLOAT_2_UCHAR
				#undef UCHAR_2_FLOAT
			}
			y++;
		}

		penX += fontInfo.font->glyph->advance.x >> 6;
		glyphIndexPrev = glyphIndex;
	}

	return penX;
}

GXVoid GXFontRenderer::Render ( const GXFontInfo &fontInfo )
{
	glBindTexture ( GL_TEXTURE_2D, fontInfo.textureID );
	glTexImage2D ( GL_TEXTURE_2D, 0, fontInfo.texture->internalFormat, fontInfo.texture->width, fontInfo.texture->height, 0, fontInfo.texture->format, fontInfo.texture->type, data );
	glBindTexture ( GL_TEXTURE_2D, 0 );
}

GXUInt GXFontRenderer::GetTextRasterLength ( const FT_Face font, GXUInt fontSize, GXBool kerning, const GXWChar* fmt, va_list op )
{
	if ( GXFtSetPixelSizes ( font, fontSize, fontSize ) )
		GXLogW ( L"GXFontRenderer::GetTextRasterLength::Error - Не удалось выставить размер шрифта %i для текста с форматирующей строкой [%s]\n", fontSize, fmt );

	FT_UInt glyphIndex;
	FT_UInt glyphIndexPrev = 0;

	FT_Bool supportKerning = FT_HAS_KERNING ( font );

	GXWChar buffer[ 1024 ];
	vswprintf ( buffer, 1023, fmt, op );

	GXUInt len = wcslen ( buffer );

	GXUInt penX = 0;

	for ( GXUInt i = 0; i < len; i++ )
	{
		glyphIndex = GXFtGetCharIndex ( font, buffer[ i ] );

		if ( supportKerning && glyphIndexPrev && kerning )
		{
			FT_Vector delta;

			GXFtGetKerning ( font, glyphIndexPrev, glyphIndex, FT_KERNING_DEFAULT, &delta );

			penX += delta.x >> 6;
		}

		if ( GXFtLoadGlyph ( font, glyphIndex, FT_LOAD_RENDER ) )
			GXLogW ( L"GXFontRenderer::GetTextRasterLength::Error - FT_Load_Glyph провалился в строке [%s] на символе #%i (%c)\n", buffer, i, buffer[ i ] );

		if ( GXFtRenderGlyph ( font->glyph, FT_RENDER_MODE_NORMAL ) )
			GXLogW ( L"GXFontRenderer::GetTextRasterLength::Error - FT_Render_Glyph провалился в строке [%s] на символе #%i (%c)\n", buffer, i, buffer[ i ] );
			
		penX += font->glyph->advance.x >> 6;
		glyphIndexPrev = glyphIndex;
	}

	return penX;
}

GXVoid GXFontRenderer::DumpGlyphToFile ( const GXWChar* fileName, const FT_Bitmap &bitmap )
{
	GXUInt total = bitmap.width * bitmap.rows + 2 * ( bitmap.rows );
	GXUChar* buffer = (GXUChar*)malloc ( total );

	GXUInt offset = 0;

	for ( GXInt i = 0; i < bitmap.rows; i++ )
	{
		memcpy ( buffer + offset, bitmap.buffer + i * bitmap.width, bitmap.width );
		offset += bitmap.width;
		buffer[ offset ] = 13;		//CR
		offset++;
		buffer[ offset ] = 10;		//LF
		offset++;
	}

	GXWriteToFile ( fileName, buffer, total );

	free ( buffer );
}