//version 1.1

#ifndef GX_MTR_STRUCTS
#define	GX_MTR_STRUCTS


#include <GXCommon/GXCommon.h>


#define	GX_ID_NOT_ASSIGNED	0xFFFFFFFF


struct GXTextureHeader
{
	GXUShort	usWidth;
	GXUShort	usHeight;
	GXBool		bIsAlpha;
	GXShort		fileNameOffset;
	GXShort		cacheFileNameOffset;
};

struct GXShaderHeader
{
	GXUInt	vShaderFileNameOffset;
	GXUInt	gShaderFileNameOffset;
	GXUInt	fShaderFileNameOffset;
};

struct GXMaterialHeader
{
	GXUShort	numTextures;
	GXUInt		texturesOffset;
	GXUShort	numShaders;
	GXUInt		shadersOffset;
};


//--------------------------------------------------------------

struct GXTextureInfo
{
	GXUInt		uiId;
	GXUShort	usWidth;
	GXUShort	usHeight;
	GXBool		bIsAlpha;
	GXWChar*	fileName;
	GXWChar*	cacheFileName;

	GXTextureInfo ();
	GXVoid GXCALL Cleanup ();
};

struct GXShaderInfo
{
	GXUInt			uiId;
	GXBool			isSamplersTuned;
	GXWChar*		vShaderFileName;
	GXWChar*		gShaderFileName;
	GXWChar*		fShaderFileName;

	GXShaderInfo ();
	GXVoid Cleanup ();
};

struct GXMaterialInfo
{
	GXUShort			numTextures;
	GXTextureInfo*		textures;
	GXUShort			numShaders;
	GXShaderInfo*		shaders;

	GXMaterialInfo ();
	GXVoid Cleanup ();
};


#endif	//GX_MTR_STRUCTS