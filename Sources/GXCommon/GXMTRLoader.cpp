//version 1.2

#include <GXCommon/GXMTRLoader.h>


GXVoid GXCALL GXLoadMTR ( const GXWChar* fileName, GXMaterialInfo &out_mat_info )
{
	GXChar* buffer;
	GXUInt size;

	if ( !GXLoadFile ( fileName, (GXVoid**)&buffer, size, GX_TRUE ) )
	{
		GXDebugBox ( L"GXLoadMTR::Error - не могу загрузить файл" );
		GXLogW ( L"GXLoadMTR::Error - не могу загрузить файл %s\n", fileName );
	}

	GXMaterialHeader header;
	memcpy ( &header, buffer, sizeof ( GXMaterialHeader ) );
	out_mat_info.numTextures = header.numTextures;
	out_mat_info.numShaders = header.numShaders;

	if ( out_mat_info.numTextures )
	{
		out_mat_info.textures = (GXTextureInfo*)malloc ( out_mat_info.numTextures * sizeof ( GXTextureInfo ) );
		GXTextureHeader* h = (GXTextureHeader*)( buffer + sizeof ( GXMaterialHeader ) );

		for ( GXUShort i = 0; i < out_mat_info.numTextures; i++ )
		{
			out_mat_info.textures[ i ].usWidth = h->usWidth;
			out_mat_info.textures[ i ].usHeight = h->usHeight;
			out_mat_info.textures[ i ].bIsAlpha = h->bIsAlpha;

			GXWChar* str = (GXWChar*)( buffer + h->fileNameOffset );
			GXUShort len = sizeof ( GXWChar ) * ( wcslen ( str ) + 1 );
			out_mat_info.textures[ i ].fileName = (GXWChar*)malloc ( len );
			memcpy ( out_mat_info.textures[ i ].fileName, str, len );

			str = (GXWChar*)( buffer + h->cacheFileNameOffset );
			len = sizeof ( GXWChar ) * ( wcslen ( str ) + 1 );
			out_mat_info.textures[ i ].cacheFileName = (GXWChar*)malloc ( len );
			memcpy ( out_mat_info.textures[ i ].cacheFileName, str, len );

			h++;
		}
	}

	if ( out_mat_info.numShaders )
	{
		out_mat_info.shaders = (GXShaderInfo*)malloc ( out_mat_info.numShaders * sizeof ( GXShaderInfo ) );
		GXShaderHeader* h = (GXShaderHeader*)( buffer + sizeof ( GXMaterialHeader ) + out_mat_info.numTextures * sizeof ( GXTextureHeader ) );
		for ( GXUShort i = 0; i < out_mat_info.numShaders; i++ )
		{
			GXWChar* str = (GXWChar*)( buffer + h->vShaderFileNameOffset );
			GXUShort len = sizeof ( GXWChar ) * ( wcslen ( str ) + 1 );
			out_mat_info.shaders[ i ].vShaderFileName = (GXWChar*)malloc ( len );
			memcpy ( out_mat_info.shaders[ i ].vShaderFileName, str, len );

			str = (GXWChar*)( buffer + h->gShaderFileNameOffset );
			if ( *str )
			{
				len = sizeof ( GXWChar ) * ( wcslen ( str ) + 1 );
				out_mat_info.shaders[ i ].gShaderFileName = (GXWChar*)malloc ( len );
				memcpy ( out_mat_info.shaders[ i ].gShaderFileName, str, len );
			}
			else
			{
				out_mat_info.shaders[ i ].gShaderFileName = 0;
			}

			str = (GXWChar*)( buffer + h->fShaderFileNameOffset );
			len = sizeof ( GXWChar ) * ( wcslen ( str ) + 1 );
			out_mat_info.shaders[ i ].fShaderFileName = (GXWChar*)malloc ( len );
			memcpy ( out_mat_info.shaders[ i ].fShaderFileName, str, len );

			h++;
		}
	}

	GXSafeFree ( buffer );
}