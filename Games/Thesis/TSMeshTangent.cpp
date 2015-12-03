//version 1.1

#include "TSMeshTangent.h"
#include <GXEngine/GXGlobals.h>


TSMeshTangent::TSMeshTangent ( const GXWChar* meshFile, const GXWChar* materialFile, GXBool twoSided )
{
	this->twoSided = twoSided;

	GXUInt size = sizeof ( GXWChar ) * ( wcslen ( meshFile ) + 1 );
	this->meshFile = (GXWChar*)malloc ( size );
	memcpy ( this->meshFile, meshFile, size );

	size = sizeof ( GXWChar ) * ( wcslen ( materialFile ) + 1 );
	this->materialFile = (GXWChar*)malloc ( size );
	memcpy ( this->materialFile, materialFile, size );

	isHidden = GX_FALSE;
	isDelete = GX_FALSE;
}

GXVoid TSMeshTangent::Draw ()
{
	if ( vaoInfo.vao == 0 )
		InitGraphicalResources ();

	if ( isDelete )
	{
		delete this;
		return;
	}

	if ( isHidden ) return;
	if ( !IsVisible () ) return;

	const GXMat4& view_mat = gx_ActiveCamera->GetViewMatrix ();
	GXMat4 mod_view_mat;
	GXMulMat4Mat4 ( mod_view_mat, mod_mat, view_mat );

	const GXMat4& view_proj_mat = gx_ActiveCamera->GetViewProjectionMatrix ();
	GXMat4 mod_view_proj_mat;
	GXMulMat4Mat4 ( mod_view_proj_mat, mod_mat, view_proj_mat );

	glUseProgram ( mat_info.shaders[ 0 ].uiId );

	glUniformMatrix4fv ( mod_view_proj_matLocation, 1, GL_FALSE, mod_view_proj_mat.A );
	glUniformMatrix4fv ( mod_view_matLocation, 1, GL_FALSE, mod_view_mat.A );

	glActiveTexture ( GL_TEXTURE0 );
	glBindTexture ( GL_TEXTURE_2D, mat_info.textures[ 0 ].uiId );

	glActiveTexture ( GL_TEXTURE1 );
	glBindTexture ( GL_TEXTURE_2D, mat_info.textures[ 1 ].uiId );

	glActiveTexture ( GL_TEXTURE2 );
	glBindTexture ( GL_TEXTURE_2D, mat_info.textures[ 2 ].uiId );

	glActiveTexture ( GL_TEXTURE3 );
	glBindTexture ( GL_TEXTURE_2D, mat_info.textures[ 3 ].uiId );

	glActiveTexture ( GL_TEXTURE4 );
	glBindTexture ( GL_TEXTURE_2D, mat_info.textures[ 4 ].uiId );

	glBindVertexArray ( vaoInfo.vao );
	if ( twoSided )
	{
		glDisable ( GL_CULL_FACE );
		glDrawArrays ( GL_TRIANGLES, 0, vaoInfo.numVertices );
		glEnable ( GL_CULL_FACE );
	}
	else
		glDrawArrays ( GL_TRIANGLES, 0, vaoInfo.numVertices );
}

GXVoid TSMeshTangent::Hide ( GXBool hidden )
{
	isHidden = hidden;
}

GXVoid TSMeshTangent::Delete ()
{
	isDelete = GX_TRUE;
}

TSMeshTangent::~TSMeshTangent ()
{
	GXSafeFree ( meshFile );
	GXSafeFree ( materialFile );

	GXRemoveVAO ( vaoInfo );

	GXRemoveShaderProgram ( mat_info.shaders[ 0 ] );

	GXRemoveTexture ( mat_info.textures[ 0 ] );
	GXRemoveTexture ( mat_info.textures[ 1 ] );
	GXRemoveTexture ( mat_info.textures[ 2 ] );
	GXRemoveTexture ( mat_info.textures[ 3 ] );
	GXRemoveTexture ( mat_info.textures[ 4 ] );
}

GXVoid TSMeshTangent::Load3DModel ()
{
	GXLoadMTR ( materialFile, mat_info );

	GXGetVAOFromNativeStaticMesh ( vaoInfo, meshFile );
	GXGetShaderProgram ( mat_info.shaders[ 0 ] );

	GXGetTexture ( mat_info.textures[ 0 ] );
	GXGetTexture ( mat_info.textures[ 1 ] );
	GXGetTexture ( mat_info.textures[ 2 ] );
	GXGetTexture ( mat_info.textures[ 3 ] );
	GXGetTexture ( mat_info.textures[ 4 ] );
}

GXVoid TSMeshTangent::InitUniforms ()
{
	glUseProgram ( mat_info.shaders[ 0 ].uiId );

	glUniform1i ( GXGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "diffuseTexture" ), 0 );
	glUniform1i ( GXGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "emitTexture" ), 1 );
	glUniform1i ( GXGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "specPowMetalIntensTexture" ), 2 );
	glUniform1i ( GXGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "specColorTexture" ), 3 );
	glUniform1i ( GXGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "normalTexture" ), 4 );

	mod_view_proj_matLocation = GXGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "mod_view_proj_mat" );
	mod_view_matLocation = GXGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "mod_view_mat" );

	glUseProgram ( 0 );
}

GXVoid TSMeshTangent::InitGraphicalResources ()
{		
	Load3DModel ();
	InitUniforms ();

	GXSafeFree ( meshFile );
	GXSafeFree ( materialFile );
}