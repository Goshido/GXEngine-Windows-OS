#include "EMUnitActor.h"
#include <GXEngine/GXGlobals.h>
#include <GXEngine/GXSamplerUtils.h>
#include <GXEngine/GXShaderStorageExt.h>
#include <GXEngine/GXTextureStorageExt.h>
#include <new>


#define EM_OBJECT_INDEX		5


class EMUnitActorMesh : public GXMesh
{
	private:
		GXTexture		diffuseTexture;
		GXTexture		normalTexture;
		GXTexture		specularTexture;
		GXTexture		emissionTexture;

		GXUByte			object[ 4 ];

		GLint			mod_view_proj_matLocation;
		GLint			mod_view_matLocation;

		GLuint			sampler;

	public:
		EMUnitActorMesh ( const EMUnitActor* actor );
		virtual ~EMUnitActorMesh ();

		virtual GXVoid Draw ();

	protected:
		virtual GXVoid Load3DModel ();
		virtual GXVoid InitUniforms ();
};

EMUnitActorMesh::EMUnitActorMesh ( const EMUnitActor* actor )
{
	GXUInt obj = (GXUInt)actor;
	object[ 0 ] = (GXUByte)( obj & 0x000000FF );
	object[ 1 ] = (GXUByte)( ( obj >> 8 ) & 0x000000FF );
	object[ 2 ] = (GXUByte)( ( obj >> 16 ) & 0x000000FF );
	object[ 3 ] = (GXUByte)( ( obj >> 24 ) & 0x000000FF );

	Load3DModel ();
	InitUniforms ();
}

EMUnitActorMesh::~EMUnitActorMesh ()
{
	GXRemoveVAO ( vaoInfo );
	GXRemoveTextureExt ( diffuseTexture );
	GXRemoveTextureExt ( normalTexture );
	GXRemoveTextureExt ( specularTexture );
	GXRemoveTextureExt ( emissionTexture );

	glDeleteSamplers ( 1, &sampler );
}

GXVoid EMUnitActorMesh::Draw ()
{
	if ( !IsVisible () ) return;

	GXMat4 mod_view_mat;
	GXMat4 mod_view_proj_mat;

	GXMulMat4Mat4 ( mod_view_mat, mod_mat, gx_ActiveCamera->GetViewMatrix () );
	GXMulMat4Mat4 ( mod_view_proj_mat, mod_mat, gx_ActiveCamera->GetViewProjectionMatrix () );

	glUseProgram ( mat_info.shaders[ 0 ].uiId );

	glUniformMatrix4fv ( mod_view_matLocation, 1, GL_FALSE, mod_view_mat.A );
	glUniformMatrix4fv ( mod_view_proj_matLocation, 1, GL_FALSE, mod_view_proj_mat.A );

	glActiveTexture ( GL_TEXTURE0 );
	glBindSampler ( 0, sampler );
	glBindTexture ( GL_TEXTURE_2D, diffuseTexture.texObj );

	glActiveTexture ( GL_TEXTURE1 );
	glBindSampler ( 1, sampler );
	glBindTexture ( GL_TEXTURE_2D, normalTexture.texObj );

	glActiveTexture ( GL_TEXTURE2 );
	glBindSampler ( 2, sampler );
	glBindTexture ( GL_TEXTURE_2D, specularTexture.texObj );

	glActiveTexture ( GL_TEXTURE3 );
	glBindSampler ( 3, sampler );
	glBindTexture ( GL_TEXTURE_2D, emissionTexture.texObj );

	glVertexAttrib4Nub ( EM_OBJECT_INDEX, object[ 0 ], object[ 1 ], object[ 2 ], object[ 3 ] );
	glBindVertexArray ( vaoInfo.vao );

	glDrawArrays ( GL_TRIANGLES, 0, vaoInfo.numVertices );

	glBindVertexArray ( 0 );

	glActiveTexture ( GL_TEXTURE0 );
	glBindSampler ( 0, 0 );
	glBindTexture ( GL_TEXTURE_2D, 0 );

	glActiveTexture ( GL_TEXTURE1 );
	glBindSampler ( 1, 0 );
	glBindTexture ( GL_TEXTURE_2D, 0 );

	glActiveTexture ( GL_TEXTURE2 );
	glBindSampler ( 2, 0 );
	glBindTexture ( GL_TEXTURE_2D, 0 );

	glActiveTexture ( GL_TEXTURE3 );
	glBindSampler ( 3, 0 );
	glBindTexture ( GL_TEXTURE_2D, 0 );

	glUseProgram ( 0 );
}

GXVoid EMUnitActorMesh::Load3DModel ()
{
	GXGetVAOFromNativeStaticMesh ( vaoInfo, L"../3D Models/Editor Mobile/Unit Cube.stm" );

	GXLoadTexture ( L"../Textures/Editor Mobile/Default Diffuse.tex", diffuseTexture );
	GXLoadTexture ( L"../Textures/Editor Mobile/Default Normals.tex", normalTexture );
	GXLoadTexture ( L"../Textures/Editor Mobile/Default Specular.tex", specularTexture );
	GXLoadTexture ( L"../Textures/Editor Mobile/Default Emission.tex", emissionTexture );

	GXShaderInfo* shader = (GXShaderInfo*)malloc ( sizeof ( GXShaderInfo ) );
	new ( shader ) GXShaderInfo ();
	GXGetShaderProgramExt ( shader[ 0 ], L"../Shaders/Editor Mobile/StaticMesh_vs.txt", 0, L"../Shaders/Editor Mobile/StaticMesh_fs.txt" );

	mat_info.numShaders = 1;
	mat_info.shaders = shader;

	GXGLSamplerStruct ss;
	ss.anisotropy = 16.0f;
	ss.resampling = GX_SAMPLER_RESAMPLING_TRILINEAR;
	ss.wrap = GL_REPEAT;
	sampler = GXCreateSampler ( ss );
}

GXVoid EMUnitActorMesh::InitUniforms ()
{
	if ( !mat_info.shaders[ 0 ].isSamplersTuned )
	{
		const GLuint samplerIndexes[ 4 ] = { 0, 1, 2, 3 };
		const GLchar* samplerNames[ 4 ] = { "diffuseSampler", "normalSampler", "specularSampler", "emissionSampler" };

		GXTuneShaderSamplers ( mat_info.shaders[ 0 ], samplerIndexes, samplerNames, 4 );
	}

	mod_view_proj_matLocation = GXGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "mod_view_proj_mat" );
	mod_view_matLocation = GXGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "mod_view_mat" );
}

//-------------------------------------------------------------------------------------------

EMUnitActor::EMUnitActor ( const GXWChar* name, const GXMat4 &origin ):
EMActor ( name, EM_UNIT_ACTOR_CLASS, origin )
{
	mesh = new EMUnitActorMesh ( this );
	OnOriginChanged ();
}

EMUnitActor::~EMUnitActor ()
{
	delete mesh;
}

GXVoid EMUnitActor::OnDrawCommonPass ()
{
	mesh->Draw ();
}

GXVoid EMUnitActor::OnOriginChanged ()
{
	mesh->SetLocation ( origin.wv );

	GXVec3 scaleFactor;
	scaleFactor.x = 1.0f / GXLengthVec3 ( origin.xv );
	scaleFactor.y = 1.0f / GXLengthVec3 ( origin.yv );
	scaleFactor.z = 1.0f / GXLengthVec3 ( origin.zv );

	GXMat4 cleanRotation;
	cleanRotation.m14 = cleanRotation.m24 = cleanRotation.m34 = 0.0f;
	cleanRotation.m41 = cleanRotation.m42 = cleanRotation.m43 = 0.0f;
	cleanRotation.m44 = 1.0f;

	cleanRotation.m11 = origin.m11 * scaleFactor.x;
	cleanRotation.m12 = origin.m12 * scaleFactor.x;
	cleanRotation.m13 = origin.m13 * scaleFactor.x;

	cleanRotation.m21 = origin.m21 * scaleFactor.y;
	cleanRotation.m22 = origin.m22 * scaleFactor.y;
	cleanRotation.m23 = origin.m23 * scaleFactor.y;

	cleanRotation.m31 = origin.m31 * scaleFactor.z;
	cleanRotation.m32 = origin.m32 * scaleFactor.z;
	cleanRotation.m33 = origin.m33 * scaleFactor.z;

	mesh->SetRotation ( cleanRotation );
}
