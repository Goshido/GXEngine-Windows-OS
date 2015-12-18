#include "EMUnitActor.h"
#include "EMGlobals.h"
#include <GXEngine/GXSamplerUtils.h>
#include <GXEngine/GXShaderStorageExt.h>
#include <GXEngine/GXTextureStorageExt.h>
#include <GXCommon/GXStrings.h>
#include <new>


class EMUnitActorMesh : public GXMesh
{
	private:
		GXTexture		diffuseTexture;
		GXTexture		normalTexture;
		GXTexture		specularTexture;
		GXTexture		emissionTexture;

		GLint			mod_view_proj_matLocation;
		GLint			mod_view_matLocation;

		GLuint			sampler;

	public:
		EMUnitActorMesh ();
		virtual ~EMUnitActorMesh ();

		virtual GXVoid Draw ();

	protected:
		virtual GXVoid Load3DModel ();
		virtual GXVoid InitUniforms ();
};

EMUnitActorMesh::EMUnitActorMesh ()
{
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
	mesh = new EMUnitActorMesh ();
	OnOriginChanged ();
}

EMUnitActor::~EMUnitActor ()
{
	delete mesh;
}

GXVoid EMUnitActor::OnDrawCommonPass ()
{
	em_Renderer->SetObjectMask ( (GXUInt)this );
	mesh->Draw ();
}

GXVoid EMUnitActor::OnSave ( GXUByte** data )
{
	EMActorHeader* header = (EMActorHeader*)data;

	header->type = type;
	memcpy ( &header->origin, &origin, sizeof ( GXMat4 ) );
	header->isVisible = isVisible;
	header->nameOffset = sizeof ( EMActorHeader );

	GXUTF8* nameUTF8;
	GXUInt nameSize = GXToUTF8 ( &nameUTF8, name );

	memcpy ( data + sizeof ( EMActorHeader ), nameUTF8, nameSize );
	free ( nameUTF8 );

	header->size = sizeof ( EMActorHeader ) + nameSize;
}

GXVoid EMUnitActor::OnLoad ( const GXUByte* data )
{
	EMActorHeader* header = (EMActorHeader*)data;

	isVisible = header->isVisible;

	GXSafeFree ( name );
	GXToWcs ( &name, (GXUTF8*)( data + header->nameOffset ) );

	memcpy ( &origin, &header->origin, sizeof ( GXMat4 ) );
}

GXUInt EMUnitActor::OnRequeredSaveSize ()
{
	GXUInt total = sizeof ( EMActorHeader );

	GXUTF8* nameUTF8;
	total += GXToUTF8 ( &nameUTF8, name );

	free ( nameUTF8 );

	return total;
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
