//version 1.3

#include "TSShadowmap.h"
#include <GXEngine/GXTextureUtils.h>
#include <GXCommon/GXMTRLoader.h>
#include "TSLight.h"


#define TS_WRONG_INDEX	0xFFFFFFFF


TSShadowmap::TSShadowmap ()
{
	maxShadowCasters = 32;
	numShadowCasters = 0;
	shadowCasters = (GXMesh**)malloc ( sizeof ( GXMesh* ) * maxShadowCasters );

	GXGLTextureStruct ts;
	ts.data = 0;
	ts.format = GL_DEPTH_COMPONENT;
	ts.internalFormat = GL_DEPTH_COMPONENT32;
	ts.type = GL_UNSIGNED_INT;
	ts.wrap = GL_CLAMP_TO_EDGE;
	ts.height = width = 1024;
	ts.width = height = 1024;

	shadowmap = GXCreateTexture ( ts );
	glBindTexture ( GL_TEXTURE_2D, shadowmap );
	glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE );
	glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

	glGenFramebuffers ( 1, &fbo );
	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );

	glFramebufferTexture ( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowmap, 0 );

	glDrawBuffer ( GL_NONE );
	glReadBuffer ( GL_NONE );

	GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );
	if ( status != GL_FRAMEBUFFER_COMPLETE )
	{
		GXLogW ( L"TSShadowmap::TSShadowma::Error - Что-то не так с FBO\n" );
		MessageBoxW ( 0, L"Что-то не так с FBO", L"Ошибка", MB_ICONERROR );
	}

	glBindFramebuffer ( GL_FRAMEBUFFER, 0 );

	LoadShaders ();
}

TSShadowmap::~TSShadowmap ()
{
	free ( shadowCasters );

	GXRemoveShaderProgram ( matInfo.shaders[ 0 ] );
	matInfo.Cleanup ();

	glBindTexture ( GL_TEXTURE_2D, 0 );
	glBindFramebuffer ( GL_FRAMEBUFFER, 0 );

	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowmap, 0 );

	glDeleteTextures ( 1, &shadowmap );

	glBindFramebuffer ( GL_FRAMEBUFFER, 0 );
	glDeleteFramebuffers ( 1, &fbo );
}

GXVoid TSShadowmap::ResetShadowmap ()
{
	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );
	glClear ( GL_DEPTH_BUFFER_BIT );
}

GXVoid TSShadowmap::StartShadowmapPass ()
{
	glCullFace ( GL_FRONT );

	glEnable ( GL_DEPTH_TEST );
	glDepthMask ( GL_TRUE );
	glDisable ( GL_STENCIL_TEST );

	glDisable ( GL_BLEND );

	glViewport ( 0, 0, width, height );
	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );

	glDrawBuffer ( GL_NONE );
	glReadBuffer ( GL_NONE );
}

GXVoid TSShadowmap::FinishShadowmapPass ()
{
	glCullFace ( GL_BACK );

	glEnable ( GL_STENCIL_TEST );
	glDisable ( GL_DEPTH_TEST );
	glDepthMask ( GL_FALSE );

	glEnable ( GL_BLEND );
}

GLuint TSShadowmap::GetShadowmapFrom ( TSSpotlight* spot )
{
	glClear ( GL_DEPTH_BUFFER_BIT );

	const GXMat4& view_proj_mat = spot->GetViewProjectionMatrix ();

	GXMat4 mod_view_proj_mat;

	glUseProgram ( matInfo.shaders[ 0 ].uiId );

	for ( GXUInt i = 0; i < numShadowCasters; i++ )
	{
		const GXAABB& bounds = shadowCasters[ i ]->GetBoundsWorld ();

		GXProjectionClipPlanes cp ( view_proj_mat );
		if ( !cp.IsVisible ( bounds ) ) continue;

		const GXVAOInfo& vaoInfo = shadowCasters[ i ]->GetMeshVAOInfo ();

		const GXMat4& mod_mat = shadowCasters[ i ]->GetModelMatrix ();
		GXMulMat4Mat4 ( mod_view_proj_mat, mod_mat, view_proj_mat );

		glUniformMatrix4fv ( mod_view_proj_matLocation, 1, GL_FALSE, mod_view_proj_mat.A );

		glBindVertexArray ( vaoInfo.vao );
		glDrawArrays ( GL_TRIANGLES, 0, vaoInfo.numVertices );
	}

	return shadowmap;
}

GXVoid TSShadowmap::IncludeShadowCaster ( GXMesh* caster )
{
	GXUInt insert = numShadowCasters;
	numShadowCasters++;

	if ( numShadowCasters >= maxShadowCasters )
	{
		maxShadowCasters += 32;

		GXMesh** temp = (GXMesh**)malloc ( sizeof ( GXMesh* ) * maxShadowCasters );
		memcpy ( temp, shadowCasters, sizeof ( GXMesh* ) * insert );

		free ( shadowCasters );
		shadowCasters = temp;
	}

	shadowCasters[ insert ] = caster;
}

GXVoid TSShadowmap::ExcludeShadowCaster ( GXMesh* caster )
{
	GXUInt i = FindShadowCasterIndex ( caster );

	if ( i == TS_WRONG_INDEX ) return;

	memmove ( shadowCasters + i, shadowCasters + i + 1, ( numShadowCasters - i - 1 ) * sizeof ( GXMesh* ) );

	numShadowCasters--;
}

GXVoid TSShadowmap::LoadShaders ()
{
	GXLoadMTR ( L"../Materials/Thesis/Shadowmap.mtr", matInfo );
	GXGetShaderProgram ( matInfo.shaders[ 0 ] );

	mod_view_proj_matLocation = GXGetUniformLocation ( matInfo.shaders[ 0 ].uiId, "mod_view_proj_mat" );
}

GXUInt TSShadowmap::FindShadowCasterIndex ( const GXMesh* caster )
{
	for ( GXUInt i = 0; i < numShadowCasters; i++ )
	{
		if ( shadowCasters[ i ] == caster )
			return i;
	}

	return TS_WRONG_INDEX;
}