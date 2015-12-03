//versino 1.1

#include "TSSprite.h"
#include <GXEngine/GXGlobals.h>


TSSprite::TSSprite ( GXWChar* material )
{
	GXUInt size = sizeof ( GXWChar ) * ( wcslen ( material ) + 1 );
	this->material = (GXWChar*)malloc ( size );
	memcpy ( this->material, material, size );

	isDelete = GX_FALSE;
}

GXVoid TSSprite::Delete ()
{
	isDelete = GX_TRUE;
}

GXVoid TSSprite::Draw ()
{
	if ( vaoInfo.vao == 0 )
		InitGraphicalResources ();

	if ( isDelete )
	{
		delete this;
		return;
	}

	GXMat4 mod_view_proj_mat;
	const GXMat4* view_proj_mat = gx_ActiveCamera->GetViewProjectionMatrixPtr ();

	GXMulMat4Mat4 ( mod_view_proj_mat, mod_mat, *view_proj_mat );

	glUseProgram ( mat_info.shaders[ 0 ].uiId );

	glUniformMatrix4fv ( mod_view_proj_matLocation, 1, GL_FALSE, mod_view_proj_mat.A );

	glActiveTexture ( GL_TEXTURE0 );
	glBindTexture ( GL_TEXTURE_2D, mat_info.textures[ 0 ].uiId );

	glDepthMask ( GL_FALSE );
	glEnable ( GL_BLEND );
	glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	if ( isTwoFaced )
	{
		glDisable ( GL_CULL_FACE );

		glBindVertexArray ( vaoInfo.vao );
		glDrawArrays ( GL_TRIANGLES, 0, vaoInfo.numVertices );

		glEnable ( GL_CULL_FACE );
	}
	else
	{
		glBindVertexArray ( vaoInfo.vao );
		glDrawArrays ( GL_TRIANGLES, 0, vaoInfo.numVertices );
	}

	glDepthMask ( GL_TRUE );
	glDisable ( GL_BLEND );
}

GXVoid TSSprite::SetTwoFaced ( GXBool twoFaced )
{
	isTwoFaced = twoFaced;
}

TSSprite::~TSSprite ()
{
	GXSafeFree ( material );

	GXRemoveVAO ( vaoInfo );
	GXRemoveShaderProgram ( mat_info.shaders[ 0 ] );
	GXRemoveTexture ( mat_info.textures[ 0 ] );
}

GXVoid TSSprite::Load3DModel ()
{
	GXGetVAOFromOBJ ( vaoInfo, L"../3D Models/System/ScreenQuad.obj", L"../3D Models/System/Cache/ScreenQuad.cache" );
	
	GXLoadMTR ( material, mat_info );
	GXGetShaderProgram ( mat_info.shaders[ 0 ] );
	GXGetTexture ( mat_info.textures[ 0 ] );

	GXSafeFree ( material );
}

GXVoid TSSprite::InitUniforms ()
{
	glUseProgram ( mat_info.shaders[ 0 ].uiId );

	mod_view_proj_matLocation = GXGetUniformLocation ( mat_info.shaders[0].uiId, "mod_view_proj_mat" );
	glUniform1i ( GXGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "Texture" ), 0 );

	glUseProgram ( 0 );
}

GXVoid TSSprite::InitGraphicalResources ()
{
	Load3DModel ();
	InitUniforms ();
}