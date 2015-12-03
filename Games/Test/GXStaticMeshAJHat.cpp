//version 1.7

#include "GXStaticMeshAJHat.h"


GXStaticMeshAJHat::GXStaticMeshAJHat ()
{
	Load3DModel ();
	InitUniforms ();
}

GXStaticMeshAJHat::~GXStaticMeshAJHat ()
{
	GXRemoveVAO ( vaoInfo );

	GXRemoveTexture ( mat_info.textures[ 0 ] );
	GXRemoveTexture ( mat_info.textures[ 1 ] );

	GXRemoveShaderProgram ( mat_info.shaders[ 0 ] );
}

GXVoid GXStaticMeshAJHat::InitUniforms ()
{
	glUseProgram ( mat_info.shaders[ 0 ].uiId );

	glUniform1i ( GXGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "textureDiffuse" ), 0 );
	glUniform1i ( GXGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "textureFiguration" ), 1 );
	mod_view_proj_mat_Location = GXGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "mod_view_proj_mat" );

	glUseProgram ( 0 );
}

GXVoid GXStaticMeshAJHat::Draw ()
{
	const GXMat4* view_proj_mat = gx_ActiveCamera->GetViewProjectionMatrixPtr ();
	GXMat4 mod_view_proj_mat;
	GXMulMat4Mat4 ( mod_view_proj_mat, mod_mat, *view_proj_mat );

	glUseProgram ( mat_info.shaders[ 0 ].uiId );
	glUniformMatrix4fv ( mod_view_proj_mat_Location, 1, GL_FALSE, mod_view_proj_mat.A );

	glActiveTexture ( GL_TEXTURE0 );
	glBindTexture ( GL_TEXTURE_2D, mat_info.textures[ 0 ].uiId );
	glActiveTexture ( GL_TEXTURE1 );
	glBindTexture ( GL_TEXTURE_2D, mat_info.textures[ 1 ].uiId );

	glBindVertexArray ( vaoInfo.vao );
	glDrawArrays ( GL_TRIANGLES, 0, vaoInfo.numVertices );

	glUseProgram ( 0 );
}

GXVoid GXStaticMeshAJHat::Load3DModel ()
{
	GXLoadMTR ( L"../Materials/Test/Applejack_s_Hat.mtr", mat_info );

	GXGetTexture ( mat_info.textures[ 0 ] );
	GXGetTexture ( mat_info.textures[ 1 ] );
	GXGetShaderProgram ( mat_info.shaders[ 0 ] );

	GXGetVAOFromOBJ ( vaoInfo, L"../3D Models/Test/Applejack_s_Hat.obj", L"../3D Models/Test/Cache/Applejack_s_Hat.cache" );
}