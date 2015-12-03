//version 1.2

#include "TSCamaro.h"
#include <GXEngine/GXGlobals.h>


TSCamaro::TSCamaro ()
{
	Load3DModel ();
	InitUniforms ();
}

TSCamaro::~TSCamaro ()
{
	GXRemoveVAO ( vaoInfo );

	GXRemoveShaderProgram ( mat_info.shaders[ 0 ] );

	GXRemoveTexture ( mat_info.textures[ 0 ] );
	GXRemoveTexture ( mat_info.textures[ 1 ] );
	GXRemoveTexture ( mat_info.textures[ 2 ] );
	GXRemoveTexture ( mat_info.textures[ 3 ] );
}

GXVoid TSCamaro::Draw ()
{
	const GXMat4* view_mat = gx_ActiveCamera->GetViewMatrixPtr ();
	GXMat4 mod_view_mat;
	GXMulMat4Mat4 ( mod_view_mat, mod_mat, *view_mat );

	const GXMat4* view_proj_mat = gx_ActiveCamera->GetViewProjectionMatrixPtr ();
	GXMat4 mod_view_proj_mat;
	GXMulMat4Mat4 ( mod_view_proj_mat, mod_mat, *view_proj_mat );

	glUseProgram ( mat_info.shaders[ 0 ].uiId );

	glUniformMatrix4fv ( mod_view_matLocation, 1, GL_FALSE, mod_view_mat.A );
	glUniformMatrix4fv ( mod_view_proj_matLocation, 1, GL_FALSE, mod_view_proj_mat.A );

	glActiveTexture ( GL_TEXTURE0 );
	glBindTexture ( GL_TEXTURE_2D, mat_info.textures[ 0 ].uiId );

	glActiveTexture ( GL_TEXTURE1 );
	glBindTexture ( GL_TEXTURE_2D, mat_info.textures[ 1 ].uiId );

	glActiveTexture ( GL_TEXTURE2 );
	glBindTexture ( GL_TEXTURE_2D, mat_info.textures[ 2 ].uiId );

	glActiveTexture ( GL_TEXTURE3 );
	glBindTexture ( GL_TEXTURE_2D, mat_info.textures[ 3 ].uiId );

	glBindVertexArray ( vaoInfo.vao );
	glDrawArrays ( GL_TRIANGLES, 0, vaoInfo.numVertices );
}

GXVoid TSCamaro::Load3DModel ()
{
	GXLoadMTR ( L"../Materials/TSCamaro.mtr", mat_info );
	GXGetVAOFromOBJ ( vaoInfo, L"../3D Models/Camaro.obj", L"../3D Models/Cached Models/Camaro.cache" );
	GXGetShaderProgram ( mat_info.shaders[ 0 ] );
	GXGetTexture ( mat_info.textures[ 0 ] );
	GXGetTexture ( mat_info.textures[ 1 ] );
	GXGetTexture ( mat_info.textures[ 2 ] );
	GXGetTexture ( mat_info.textures[ 3 ] );
}

GXVoid TSCamaro::InitUniforms ()
{
	glUseProgram ( mat_info.shaders[ 0 ].uiId );

	glUniform1i ( GXGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "diffuseTexture" ), 0 );
	glUniform1i ( GXGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "emitTexture" ), 1 );
	glUniform1i ( GXGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "specPowMetalIntensTexture" ), 2 );
	glUniform1i ( GXGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "specColorTexture" ), 3 );

	mod_view_proj_matLocation = GXGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "mod_view_proj_mat" );
	mod_view_matLocation = GXGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "mod_view_mat" );

	glUseProgram ( 0 );
}