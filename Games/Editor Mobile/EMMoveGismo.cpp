#include "EMMoveGismo.h"
#include <GXEngine/GXShaderStorageExt.h>
#include <GXEngine/GXGlobals.h>


EMMoveGismo::EMMoveGismo ()
{
	isDeleted = GX_FALSE;
	isVisible = GX_TRUE;

	mod_view_proj_matLocation = -1;
}

GXVoid EMMoveGismo::Delete ()
{
	isDeleted = GX_TRUE;
}

GXVoid EMMoveGismo::Hide ()
{
	isVisible = GX_FALSE;
}

GXVoid EMMoveGismo::Show ()
{
	isVisible = GX_TRUE;
}

GXVoid EMMoveGismo::Draw ()
{
	if ( vaoInfo.vao == 0 )
	{
		Load3DModel ();
		InitUniforms ();
	}

	if ( isDeleted )
	{
		delete this;
		return;
	}

	if ( !isVisible ) return;

	glUseProgram ( mat_info.shaders[ 0 ].uiId );

	const GXMat4& view_proj_mat = gx_ActiveCamera->GetViewProjectionMatrix ();
	GXMat4 mod_view_proj_mat;
	GXMulMat4Mat4 ( mod_view_proj_mat, mod_mat, view_proj_mat );

	glUniformMatrix4fv ( mod_view_proj_matLocation, 1, GL_FALSE, mod_view_proj_mat.A );

	glActiveTexture ( GL_TEXTURE0 );
	glBindTexture ( GL_TEXTURE_2D, texture.texObj );

	glBindVertexArray ( vaoInfo.vao );
	glDrawArrays ( GL_TRIANGLES, 0, vaoInfo.numVertices );
	glBindVertexArray ( 0 );

	glBindTexture ( GL_TEXTURE_2D, 0 );
	glUseProgram ( 0 );
}

EMMoveGismo::~EMMoveGismo ()
{
	GXRemoveVAO ( vaoInfo );
	GXRemoveShaderProgramExt ( mat_info.shaders[ 0 ] );
	GXRemoveTextureExt ( texture );
}

GXVoid EMMoveGismo::Load3DModel ()
{
	GXGetVAOFromNativeStaticMesh ( vaoInfo, L"../3D Models/Editor Mobile/Move Gismo.stm" );
	UpdateBounds ();

	GXLoadTexture ( L"../Textures/Editor Mobile/Gismo Texture.tex", texture );
	glBindTexture ( GL_TEXTURE_2D, texture.texObj );
	glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameterf ( GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0f );
	glBindTexture ( GL_TEXTURE_2D, 0 );

	GXShaderInfo* si = (GXShaderInfo*)malloc ( sizeof ( GXShaderInfo ) );
	new ( si ) GXShaderInfo;

	mat_info.numShaders = 1;
	mat_info.shaders = si;

	GXGetShaderProgramExt ( mat_info.shaders[ 0 ], L"../Shaders/System/VertexAndUV_vs.txt", 0, L"../Shaders/Editor Mobile/OneSampler_fs.txt" );
}

GXVoid EMMoveGismo::InitUniforms ()
{
	if ( !mat_info.shaders[ 0 ].isSamplersTuned )
	{
		const GLuint samplerIndexes[ 1 ] = { 0 };
		const GLchar* samplerNames[ 1 ] = { "imageSampler" };

		GXTuneShaderSamplers ( mat_info.shaders[ 0 ], samplerIndexes, samplerNames, 1 );
	}

	mod_view_proj_matLocation = GXGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "mod_view_proj_mat" );
}
