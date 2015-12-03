//version 1.1

#include <GXCommon/GXMTRStructs.h>


GXTextureInfo::GXTextureInfo ()
{
	uiId = GX_ID_NOT_ASSIGNED;
	usWidth = usHeight = 0;
	bIsAlpha = GX_FALSE;
	fileName = cacheFileName = 0;
}

GXVoid GXCALL GXTextureInfo::Cleanup ()
{
	if ( fileName ) free ( fileName );
	if ( cacheFileName ) free ( cacheFileName );
}

//------------------------------------------------

GXShaderInfo::GXShaderInfo ()
{
	uiId = GX_ID_NOT_ASSIGNED;
	vShaderFileName = gShaderFileName = fShaderFileName = 0;
}

GXVoid GXShaderInfo::Cleanup ()
{
	GXSafeFree ( vShaderFileName );
	GXSafeFree ( gShaderFileName );
	GXSafeFree ( fShaderFileName );
}

//-------------------------------------------------

GXMaterialInfo::GXMaterialInfo ()
{
	textures = 0;
	shaders = 0;
	numTextures = numShaders = 0;
}

GXVoid GXMaterialInfo::Cleanup ()
{
	if ( textures )
		for ( GXUShort i = 0; i < numTextures; i++ )
			textures[ i ].Cleanup ();

	if ( shaders )
		for ( GXUShort i = 0; i < numShaders; i++ )
			shaders[ i ].Cleanup ();

	GXSafeFree ( textures );
	GXSafeFree ( shaders );
	numTextures = numShaders = 0;
}