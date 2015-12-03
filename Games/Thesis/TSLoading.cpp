//version 1.1

#include "TSLoading.h"
#include <GXEngine/GXTextureStorage.h>
#include <GXEngine/GXVAOStorage.h>
#include <GXEngine/GXShaderStorage.h>
#include <GXCommon/GXMTRLoader.h>


class TSResource
{
	protected:
		TSResource*			next;
		GLuint				object;

	public:
		TSResource ();
		virtual ~TSResource ();		
};

TSResource*		ts_loadingTop = 0;

TSResource::TSResource ()
{
	next = ts_loadingTop;
	ts_loadingTop = this;
}

TSResource::~TSResource ()
{
	ts_loadingTop = next;
}

//----------------------------------------------------------------------------------------------

class TSTexture : public TSResource
{
	public:
		TSTexture ( GXTextureInfo &info );
		virtual ~TSTexture ();
};

TSTexture::TSTexture ( GXTextureInfo &info )
{
	object = GXGetTexture ( info );
}

TSTexture::~TSTexture ()
{
	GXTextureInfo t;
	t.uiId = object;
	GXRemoveTexture ( t );
}

//----------------------------------------------------------------------------------------------

class TSShader : public TSResource
{
	public:
		TSShader ( GXShaderInfo &info );
		virtual ~TSShader ();
};

TSShader::TSShader ( GXShaderInfo &info )
{
	object = GXGetShaderProgram ( info );
}

TSShader::~TSShader ()
{
	GXShaderInfo s;
	s.uiId = object;
	GXRemoveShaderProgram ( s );
}

//----------------------------------------------------------------------------------------------

class TSTransformFeedbackShader : public TSResource
{
	public:
		TSTransformFeedbackShader ( GXShaderInfo &info, GXUChar numVaryings, const GXChar** varyings, GLenum bufferMode );
		virtual ~TSTransformFeedbackShader ();
};

TSTransformFeedbackShader::TSTransformFeedbackShader ( GXShaderInfo &info, GXUChar numVaryings, const GXChar** varyings, GLenum bufferMode )
{
	object = GXGetTransformFeedbackShaderProgram ( info, numVaryings, varyings, bufferMode );
}

TSTransformFeedbackShader::~TSTransformFeedbackShader ()
{
	GXShaderInfo s;
	s.uiId = object;
	GXRemoveShaderProgram ( s );
}

//------------------------------------------------------------------------------------------

class TSVAO : public TSResource
{
	private:
		GXVAOInfo	vaoInfo;

	public:
		TSVAO ( const GXWChar* fileName, const GXWChar* cacheFileName );
		virtual ~TSVAO ();
};

TSVAO::TSVAO ( const GXWChar* fileName, const GXWChar* cacheFileName )
{
	GXGetVAOFromOBJ ( vaoInfo, fileName, cacheFileName );
}

TSVAO::~TSVAO ()
{
	GXRemoveVAO ( vaoInfo );
}

//------------------------------------------------------------------------------------------

GXVoid GXCALL TSLoadMaterialResources ( const GXWChar* fileName )
{
	GXMaterialInfo mi;

	GXLoadMTR ( fileName, mi );
	for ( GXUInt i = 0; i < mi.numShaders; i++ )
		new TSShader ( mi.shaders[ i ] );

	for ( GXUInt i = 0; i < mi.numTextures; i++ )
		new TSTexture ( mi.textures[ i ] );

	mi.Cleanup ();
}

//------------------------------------------------------------------------------------------

GXVoid GXCALL TSOnLoading ()
{
	TSLoadMaterialResources ( L"../Materials/Thesis/Arizona_Ground.mtr" );
	TSLoadMaterialResources ( L"../Materials/Thesis/Arizona_Skybox.mtr" );
	TSLoadMaterialResources ( L"../Materials/Thesis/Bloom_Effect.mtr" );
	TSLoadMaterialResources ( L"../Materials/Thesis/Brown_Wood_Box.mtr" );
	TSLoadMaterialResources ( L"../Materials/Thesis/Cannon.mtr" );
	TSLoadMaterialResources ( L"../Materials/Thesis/Cannonball.mtr" );
	TSLoadMaterialResources ( L"../Materials/Thesis/Gismo_Axis.mtr" );
	TSLoadMaterialResources ( L"../Materials/Thesis/Green_Wood_Box.mtr" );
	TSLoadMaterialResources ( L"../Materials/Thesis/Hud_Content.mtr" );
	TSLoadMaterialResources ( L"../Materials/Thesis/Land_Cruiser_Prado.mtr" );
	TSLoadMaterialResources ( L"../Materials/Thesis/Light_Gismo.mtr" );
	TSLoadMaterialResources ( L"../Materials/Thesis/Missile.mtr" );
	TSLoadMaterialResources ( L"../Materials/Thesis/Muzzle_Flash01.mtr" );
	TSLoadMaterialResources ( L"../Materials/Thesis/Oil_Pump.mtr" );
	TSLoadMaterialResources ( L"../Materials/Thesis/Shadowmap.mtr" );
	TSLoadMaterialResources ( L"../Materials/Thesis/Wrench.mtr" );

	const GXChar* varyings[] = 
	{
		"outPosition",
		"outVelocity",
		"outLifetime"
	};

	GXMaterialInfo mi;
	GXLoadMTR ( L"../Materials/Thesis/Exhaust_Particles.mtr", mi );
	new TSTransformFeedbackShader ( mi.shaders[ 0 ], 3, varyings, GL_SEPARATE_ATTRIBS );
	mi.Cleanup ();

	new TSVAO ( L"../3D Models/Thesis/Ammo_Box.obj", L"../3D Models/Thesis/Cache/Ammo_Box.cache" );
	new TSVAO ( L"../3D Models/Demo/Ground.obj", L"../3D Models/Demo/Cache/Ground.cache" );
	new TSVAO ( L"../3D Models/Thesis/Cannon.obj", L"../3D Models/Thesis/Cache/Cannon.cache" );
	new TSVAO ( L"../3D Models/Thesis/Cube_Skybox.obj", L"../3D Models/Thesis/Cache/Cube_Skybox.cache" );
	new TSVAO ( L"../3D Models/Thesis/Land_Cruiser.obj", L"../3D Models/Thesis/Cache/Land_Cruiser.cache" );
	new TSVAO ( L"../3D Models/Thesis/Land_Cruiser_Wheel.obj", L"../3D Models/Thesis/Cache/Land_Cruiser_Wheel.cache" );
	new TSVAO ( L"../3D Models/Thesis/Missile.obj", L"../3D Models/Thesis/Cache/Missile.cache" );
	new TSVAO ( L"../3D Models/Thesis/Missile_Item.obj", L"../3D Models/Thesis/Cache/Missile_Item.cache" );
	new TSVAO ( L"../3D Models/Thesis/Missile_Pack.obj", L"../3D Models/Thesis/Cache/Missile_Pack.cache" );
	new TSVAO ( L"../3D Models/Thesis/Oil_Pump.obj", L"../3D Models/Thesis/Cache/Oil_Pump.cache" );
	new TSVAO ( L"../3D Models/Thesis/OneCone.obj", L"../3D Models/Thesis/Cache/OneCone.cache" );
	new TSVAO ( L"../3D Models/Thesis/OneSphere.obj", L"../3D Models/Thesis/Cache/OneSphere.cache" );
	new TSVAO ( L"../3D Models/Thesis/Wrench.obj", L"../3D Models/Thesis/Cache/Wrench.cache" );
}

GXVoid GXCALL TSOnUnLoading ()
{
	while ( ts_loadingTop )
		delete ts_loadingTop;
}