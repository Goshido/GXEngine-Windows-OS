//version 1.6

#include "GXStaticMeshGround.h"


GXStaticMeshGround::GXStaticMeshGround ()
{
	Load3DModel ();
	InitUniforms ();
}

GXStaticMeshGround::~GXStaticMeshGround ()
{
	GXRemoveVAO ( vaoInfo );
	GXRemoveTexture ( mat_info.textures [ 0 ] );
	GXRemoveShaderProgram ( mat_info.shaders [ 0 ] );
}

GXVoid GXStaticMeshGround::InitUniforms ()
{
	glUseProgram ( mat_info.shaders [ 0 ].uiId );
	glUniform1i ( GXGetUniformLocation ( mat_info.shaders [ 0 ].uiId, "diffuseSampler" ), 0 );
	mod_view_proj_mat_Location = GXGetUniformLocation ( mat_info.shaders [ 0 ].uiId, "mod_view_proj_mat" );
	glUseProgram ( 0 );
}

GXVoid GXStaticMeshGround::Draw ()
{
	const GXMat4* view_proj_mat = gx_ActiveCamera->GetViewProjectionMatrixPtr ();
	GXMat4 mod_view_proj_mat;
	GXMulMat4Mat4 ( mod_view_proj_mat, mod_mat, *view_proj_mat );

	glUseProgram ( mat_info.shaders [ 0 ].uiId );
	glUniformMatrix4fv ( mod_view_proj_mat_Location, 1, GL_TRUE, mod_view_proj_mat.A );
	glActiveTexture ( GL_TEXTURE0 );
	glBindTexture ( GL_TEXTURE_2D, mat_info.textures [ 0 ].uiId );

	glBindVertexArray ( vaoInfo.vao );
	glDrawArrays ( GL_TRIANGLES, 0, vaoInfo.numVertices );

	glUseProgram ( 0 );
}

GXVoid GXStaticMeshGround::Load3DModel ()
{
	GXLoadMTR ( L"../Materials/Test/Ground.mtr", mat_info );

	GXGetTexture ( mat_info.textures[ 0 ] );

	GXGetShaderProgram ( mat_info.shaders[ 0 ] );

	GXGetVAOFromOBJ ( vaoInfo, L"../3D Models/Test/Ground.obj", L"../3D Models/Test/Cache/Ground.cache" );
}