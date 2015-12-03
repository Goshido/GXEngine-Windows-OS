//version 1.0


#include "TSSkeletalMesh.h"
#include "TSGlobals.h"
#include <GXCommon/GXStrings.h>


TSSkeletalMesh::TSSkeletalMesh ( const GXWChar* fileName, const GXWChar* materialName )
{
	isDelete = GX_FALSE;
	mod_view_proj_matLocation = mod_view_matLocation = bonesLocation = -1;

	GXWcsclone ( &this->fileName, fileName );
	GXWcsclone ( &this->materialName, materialName );
}

GXVoid TSSkeletalMesh::Draw ()
{
	if ( skmInfo.vbo == 0 )
	{
		Load3DModel ();
		InitUniforms ();
	}

	if ( isDelete )
	{
		delete this;
		return;
	}

	if ( !IsVisible () ) return;

	InitPose ();

	glUseProgram ( mat_info.shaders[ 0 ].uiId );

	for ( GXUByte i = 0; i < 5; i++ )
	{
		glActiveTexture ( GL_TEXTURE0 + i );
		glBindTexture ( GL_TEXTURE_2D, mat_info.textures[ i ].uiId );
	}

	GXMat4 mod_view_proj_mat;
	GXMat4 mod_view_mat;
	const GXMat4& view_proj_mat = gx_ActiveCamera->GetViewProjectionMatrix ();
	const GXMat4& view_mat = gx_ActiveCamera->GetViewMatrix ();
	GXMulMat4Mat4 ( mod_view_proj_mat, mod_mat, view_proj_mat );
	GXMulMat4Mat4 ( mod_view_mat, mod_mat, view_mat );

	glUniformMatrix4fv ( mod_view_proj_matLocation, 1, GL_FALSE, mod_view_proj_mat.A );
	glUniformMatrix4fv ( mod_view_matLocation, 1, GL_FALSE, mod_view_mat.A );
	
	glUniform1fv ( bonesLocation, GX_MAX_BONES_FLOATS, (const GLfloat*)vertexTransform );

	glBindVertexArray ( skmInfo.vao );
	glDrawArrays ( GL_TRIANGLES, 0, skmInfo.numVertices );
	glBindVertexArray ( 0 );

	for ( GXUByte i = 0; i < 5; i++ )
	{
		glActiveTexture ( GL_TEXTURE0 + i );
		glBindTexture ( GL_TEXTURE_2D, 0 );
	}

	glUseProgram ( 0 );
}

GXVoid TSSkeletalMesh::Delete ()
{
	isDelete = GX_TRUE;
}

TSSkeletalMesh::~TSSkeletalMesh ()
{
	free ( fileName );
	GXRemoveSkeletalMesh ( skmInfo );

	free ( materialName );
	GXRemoveTexture ( mat_info.textures[ 0 ] );
	GXRemoveTexture ( mat_info.textures[ 1 ] );
	GXRemoveTexture ( mat_info.textures[ 2 ] );
	GXRemoveTexture ( mat_info.textures[ 3 ] );
	GXRemoveTexture ( mat_info.textures[ 4 ] );

	GXRemoveShaderProgram ( mat_info.shaders[ 0 ] );
}

GXVoid TSSkeletalMesh::Load3DModel ()
{
	GXGetSkeletalMesh ( fileName, skmInfo );

	GXLoadMTR ( materialName, mat_info );
	GXGetTexture ( mat_info.textures[ 0 ] );
	GXGetTexture ( mat_info.textures[ 1 ] );
	GXGetTexture ( mat_info.textures[ 2 ] );
	GXGetTexture ( mat_info.textures[ 3 ] );
	GXGetTexture ( mat_info.textures[ 4 ] );

	GXGetShaderProgram ( mat_info.shaders[ 0 ] );

	InitReferencePose ();
}

GXVoid TSSkeletalMesh::InitUniforms ()
{
	glUseProgram ( mat_info.shaders[ 0 ].uiId );

	glUniform1i ( GXGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "diffuseTexture" ), 0 );
	glUniform1i ( GXGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "emitTexture" ), 1 );
	glUniform1i ( GXGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "specPowMetalIntensTexture" ), 2 );
	glUniform1i ( GXGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "specColorTexture" ), 3 );
	glUniform1i ( GXGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "normalTexture" ), 4 );

	mod_view_proj_matLocation = GXGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "mod_view_proj_mat" );
	mod_view_matLocation = GXGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "mod_view_mat" );
	bonesLocation = GXGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "bones" );

	glUseProgram ( 0 );
}
