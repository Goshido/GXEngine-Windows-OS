//version 1.2

#include <GXCommon/GXMTRSaver.h>
#include <GXEngine/GXTGALoader.h>


GXVoid GXCALL GXExportMTR ( GXWChar* fileName, const GXMaterialInfo &mat_info )
{
	GXMaterialHeader header;
	GXUInt totalHeaderSize = sizeof ( GXMaterialHeader ) + mat_info.numTextures * sizeof ( GXTextureHeader ) + mat_info.numShaders * sizeof ( GXShaderHeader );
	header.numTextures = mat_info.numTextures;
	header.numShaders = mat_info.numShaders;
	header.texturesOffset = sizeof ( GXMaterialHeader );
	header.shadersOffset = header.texturesOffset + mat_info.numTextures * sizeof ( GXTextureHeader );

	GXUInt totalDataSize = 0;
	for ( GXUShort i = 0; i < mat_info.numTextures; i++ )
	{
		totalDataSize += sizeof ( GXWChar ) * ( wcslen ( mat_info.textures[ i ].fileName ) + 1 );
		totalDataSize += sizeof ( GXWChar ) * ( wcslen ( mat_info.textures[ i ].cacheFileName ) + 1 );
	}

	for ( GXUShort i = 0; i < mat_info.numShaders; i++ )
	{
		totalDataSize += sizeof ( GXWChar ) * ( wcslen ( mat_info.shaders[ i ].vShaderFileName ) + 1 );
		
		if ( mat_info.shaders[ i ].gShaderFileName )
			totalDataSize += sizeof ( GXWChar ) * ( wcslen ( mat_info.shaders[ i ].gShaderFileName ) + 1 );
		else
			totalDataSize += sizeof ( GXWChar );

		totalDataSize += sizeof ( GXWChar ) * ( wcslen ( mat_info.shaders[ i ].fShaderFileName ) + 1 );
	}

	GXChar* buffer = (GXChar*)malloc ( totalHeaderSize + totalDataSize );
	memcpy ( buffer, &header, sizeof ( GXMaterialHeader ) );
	GXUInt dataOffset = totalHeaderSize;

	GXTextureHeader* textureHeader = (GXTextureHeader*)( buffer + sizeof ( GXMaterialHeader ) );
	for ( GXUShort i = 0; i < header.numTextures; i++ )
	{
		GXUShort len = sizeof ( GXWChar ) * ( wcslen ( mat_info.textures[ i ].fileName ) + 1 );
		memcpy ( buffer + dataOffset, mat_info.textures[ i ].fileName, len );
		textureHeader->fileNameOffset = dataOffset;
		dataOffset += len;

		len = sizeof ( GXWChar ) * ( wcslen ( mat_info.textures[ i ].cacheFileName ) + 1 );
		memcpy ( buffer + dataOffset, mat_info.textures [ i ].cacheFileName, len );
		textureHeader->cacheFileNameOffset = dataOffset;
		dataOffset += len;

		FILE* file;
		_wfopen_s ( &file, mat_info.textures[ i ].fileName, L"rb\0" );

		GXTGAHeader tgaHeader;
		fread ( &tgaHeader, sizeof ( GXTGAHeader ), 1, file );

		fclose ( file );

		textureHeader->usWidth = tgaHeader.Width;
		textureHeader->usHeight = tgaHeader.Height; 
		textureHeader->bIsAlpha = tgaHeader.BitsPerPixel == 32;

		textureHeader++;
	}

	GXShaderHeader* shaderHeader = (GXShaderHeader*)( buffer + sizeof ( GXMaterialHeader ) + mat_info.numTextures * sizeof ( GXTextureHeader ) );
	for ( GXUShort i = 0; i < header.numShaders; i++ )
	{
		GXUShort len = sizeof ( GXWChar ) * ( wcslen ( mat_info.shaders[ i ].vShaderFileName ) + 1 );
		memcpy ( buffer + dataOffset, mat_info.shaders[ i ].vShaderFileName, len );
		shaderHeader->vShaderFileNameOffset = dataOffset;
		dataOffset += len;

		if ( mat_info.shaders[ i ].gShaderFileName )
		{
			len = sizeof ( GXWChar ) * ( wcslen ( mat_info.shaders[ i ].gShaderFileName ) + 1 );
			memcpy ( buffer + dataOffset, mat_info.shaders[ i ].gShaderFileName, len );
			shaderHeader->gShaderFileNameOffset = dataOffset;
			dataOffset += len;
		}
		else
		{
			GXWChar* str = (GXWChar*)( buffer + dataOffset );
			*str = 0;
			shaderHeader->gShaderFileNameOffset = dataOffset;
			dataOffset += sizeof ( GXWChar );
		}

		len = sizeof ( GXWChar ) * ( wcslen ( mat_info.shaders[ i ].fShaderFileName ) + 1 );
		memcpy ( buffer + dataOffset, mat_info.shaders[ i ].fShaderFileName, len );
		shaderHeader->fShaderFileNameOffset = dataOffset;
		dataOffset += len;

		shaderHeader++;
	}

	GXWriteToFile ( fileName, buffer, totalHeaderSize + totalDataSize );
	free ( buffer );
}

