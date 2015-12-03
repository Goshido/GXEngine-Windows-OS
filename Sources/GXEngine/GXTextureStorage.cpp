//version 1.4

#include <GXEngine/GXTextureStorage.h>
#include <GXEngine/GXTGALoader.h>
#include <GXEngine/GXTextureUtils.h>
#include <GXEngine/GXGlobals.h>
#include <GXCommon/GXStrings.h>


#define CONTAINS_ALPHA_CHANNEL	0xFF
#define NO_ALPHA_CHANNEL		0x00


struct GXTextureUnit
{
	GXInt		count_ref;
	GLuint		Texture;
	GXWChar*	fileName;
	GXUInt		Width;
	GXUInt		Height;
	GXBool		bIsAlpha;

	GXTextureUnit* next;
	GXTextureUnit* prev;
};

GXTextureUnit* gx_strg_Textures = 0;


GXVoid GXCALL AddTextureUnit ( GXTextureUnit* unit )
{
	if ( !gx_strg_Textures )
	{
		gx_strg_Textures = unit;
		unit->next = unit->prev = 0;
	}
	else
	{
		unit->next = gx_strg_Textures;
		unit->prev = 0;
		gx_strg_Textures->prev = unit;
		gx_strg_Textures = unit;
	}
}

GXTextureUnit* GXCALL FindTextureUnitByFileName ( const GXWChar* fileName )
{
	GXTextureUnit* p = gx_strg_Textures;
	while ( p )
	{
		if ( wcscmp ( p->fileName, fileName ) == 0 ) return p;
		p = p->next;
	}
	return 0;
}

GXTextureUnit* GXCALL FindTextureUnitByID ( GLuint textureID )
{
	GXTextureUnit* p = gx_strg_Textures;
	while ( p )
	{
		if ( p->Texture == textureID ) return p;
		p = p->next;
	}
	return 0;
}

GXVoid GXCALL DeleteTextureUnit ( GLuint textureID )
{
	GXTextureUnit* p = gx_strg_Textures;
	while ( p )
	{
		if ( p->Texture == textureID ) break;
		p = p->next;
	}
	if ( p == 0 ) return;
	if ( p == gx_strg_Textures )
	{
		gx_strg_Textures = p->next;
		if ( gx_strg_Textures )
			gx_strg_Textures->prev = 0;
		GXSafeDelete ( p );
	}
	else 
	{
		if ( p->next == 0)
		{
			p->prev->next = 0;
			GXSafeDelete ( p );
		}
		else
		{
			p->prev->next = p->next;
			p->next->prev = p->prev;
			GXSafeDelete ( p );
		}
	}
}

GXVoid GXCALL InitTextureUnit ( GXTextureUnit* unit, GXChar* data )
{
	GXGLTextureStruct texture_info;
	texture_info.type = GL_UNSIGNED_BYTE;
	texture_info.width = unit->Width;
	texture_info.height = unit->Height;
	texture_info.data = data;
	texture_info.wrap = GL_REPEAT;
	texture_info.resampling = gx_EngineSettings.resampling;
	texture_info.anisotropy = gx_EngineSettings.anisotropy;
	if ( unit->bIsAlpha )
	{
		texture_info.format = GL_RGBA;
		texture_info.internalFormat = GL_RGBA8;
	}
	else
	{	
		texture_info.format = GL_RGB;
		texture_info.internalFormat = GL_RGB8;
	}
	unit->Texture = GXCreateTexture ( texture_info );
}

GXBool GXCALL TryToLoadTextureFromCache ( GXWChar* cachedfile, GXTextureUnit* unit )
{
	GXUInt size;
	GXChar* cache_source;
	if ( GXLoadFile ( cachedfile, (GXVoid**)&cache_source, size, GX_FALSE ) )
	{
		InitTextureUnit ( unit, cache_source );
		GXSafeFree ( cache_source );
		return GX_TRUE;
	}
	return GX_FALSE;
}

GXUInt GXCALL PrepareTextureDataToCache ( GXUChar** data, GXUInt width, GXUInt height, GXBool bIsAlpha )
{
	GXUChar* buf = *data;
	GXUInt newFileSize;
	GXUChar* AlphaOffset;

	if ( bIsAlpha )
	{
		newFileSize = width * height * 4 + 1;
		*data = (GXUChar*)calloc ( sizeof ( GXUChar ), newFileSize );
		memcpy ( *data, buf, newFileSize - 1 );
		AlphaOffset = *data + newFileSize - 1;
		*AlphaOffset = CONTAINS_ALPHA_CHANNEL;
	}
	else
	{
		newFileSize = width * height * 3 + 1;
		*data = (GXUChar*)calloc ( sizeof ( GXUChar ), newFileSize );
		memcpy ( *data, buf, newFileSize - 1 );
		AlphaOffset = *data + newFileSize - 1;
		*AlphaOffset = NO_ALPHA_CHANNEL;
	}

	GXSafeFree ( buf );
	return newFileSize;
}

GLuint GXCALL GXGetTexture ( GXTextureInfo &tex_info )
{
	GXTextureUnit* texture = FindTextureUnitByFileName ( tex_info.fileName );
	//Если текстура уже загружена
	if ( texture )
	{
		texture->count_ref++;
		tex_info.uiId = texture->Texture;
		return texture->Texture;
	}
	//Если текстуру необходимо загрузить
	texture = new GXTextureUnit ();
	texture->count_ref = 1;
	texture->Width = tex_info.usWidth;
	texture->Height = tex_info.usHeight;
	texture->bIsAlpha = tex_info.bIsAlpha;

	GXUShort len = sizeof ( GXWChar ) * ( wcslen ( tex_info.fileName ) + 1 );
	texture->fileName = (GXWChar*)malloc ( len );
	memcpy ( texture->fileName, tex_info.fileName, len );

	GXUChar* texture_source;
	GXUInt width;
	GXUInt height;
	GXBool bIsAlpha;
	if ( TryToLoadTextureFromCache ( tex_info.cacheFileName, texture ) )
	{
		AddTextureUnit ( texture );
		tex_info.uiId = texture->Texture;
		return texture->Texture;
	}
	else
		GXLoadTGA ( tex_info.fileName, width, height, bIsAlpha, &texture_source );

	InitTextureUnit ( texture, (GXChar*)texture_source );
	GXUInt cachedFileSize = PrepareTextureDataToCache ( &texture_source, tex_info.usWidth, tex_info.usHeight, tex_info.bIsAlpha ); 
	GXWriteToFile ( tex_info.cacheFileName, texture_source, cachedFileSize );

	GXSafeFree ( texture_source );
	AddTextureUnit ( texture );
	tex_info.uiId = texture->Texture;
	return texture->Texture;
}

GXVoid GXCALL GXRemoveTexture ( GXTextureInfo &tex_info )
{
	GXTextureUnit* texture = FindTextureUnitByID ( tex_info.uiId );
	if ( !texture ) return;

	texture->count_ref--;
	if ( texture->count_ref > 0 ) return;

	free ( texture->fileName );
	DeleteTextureUnit ( texture->Texture );
	glDeleteTextures ( 1, &tex_info.uiId );
}

GXUInt GXCALL GXGetTotalTextureStorageObjects ( GXWChar** lastTexture )
{
	GXUInt objects = 0;
	*lastTexture = 0;

	for ( GXTextureUnit* p = gx_strg_Textures; p; p = p->next )
	{
		*lastTexture = p->fileName;
		objects++;
	}

	return objects;
}