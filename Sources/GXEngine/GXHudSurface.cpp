//version 1.10

#include <GXEngine/GXHudSurface.h>
#include <GXEngine/GXGlobals.h>


GXVec4	gx_hudsrfc_DefaultColor ( 0.45098f, 0.72549f, 0.0f, 1.0f );


GXHudSurface::GXHudSurface ( GXUShort width, GXUShort height )
{
	needUpdateFont = needUpdateImage = GX_FALSE;

	surfaceWidth = width;
	surfaceHeight = height;

	GXFloat aspect = width / (GXFloat)height;
	GXSetMat4Ortho ( imageProjGXMat, (GXFloat)surfaceWidth, (GXFloat)surfaceHeight, 0.1f, 20.0f );

	invAspect =  1.0f / aspect;

	Load3DModel ();
	InitUniforms ();

	GXUShort optimWidth = 1;
	GXUShort optimHeight = 1;
	while ( optimWidth < width ) optimWidth <<= 1;
	while ( optimHeight < height ) optimHeight <<= 1;

	texture.data = 0;
	texture.format = GL_RGBA;
	texture.internalFormat = GL_RGBA8;
	texture.type = GL_UNSIGNED_BYTE;
	texture.wrap = GL_CLAMP_TO_EDGE;
	texture.width = optimWidth;
	texture.height = optimHeight; 

	fontInfo.textureID = GXCreateTexture ( texture );

	fontInfo.texture = &texture;
	fontInfo.insertX = fontInfo.insertY = 0;
	fontInfo.kerning = GX_TRUE;
	fontInfo.fontSize = 14;
	fontInfo.font = 0;
	fontInfo.fontColor = &gx_hudsrfc_DefaultColor;

	glGenFramebuffers ( 1, &fbo );
	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );

	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fontInfo.textureID, 0 );
	GLenum buffer = GL_COLOR_ATTACHMENT0;
	glDrawBuffers ( 1, &buffer );

	GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );
	if ( status != GL_FRAMEBUFFER_COMPLETE )
	{
		GXLogW ( L"GXHudSurface::GXHudSurface::Error - Что-то с FBO. Код ошибки 0x%04x\n", status );
		GXDebugBox ( L"Что-то с FBO Hud поверхности" );
	}

	glBindFramebuffer ( GL_FRAMEBUFFER, 0 );

	fontRenderer.Reset ( fontInfo );
}

GXHudSurface::~GXHudSurface ()
{
	GXRemoveShaderProgram ( mat_info.shaders[ 0 ] );
	GXRemoveVAO ( vaoInfo );

	glBindTexture ( GL_TEXTURE_2D, 0 );
	glDeleteTextures ( 1, &fontInfo.textureID );

	glBindFramebuffer ( GL_FRAMEBUFFER, 0 );
	glDeleteFramebuffers ( 1, &fbo );
}

GXVoid GXHudSurface::Reset ()
{
	GLuint oldFBO;
	glGetIntegerv ( GL_FRAMEBUFFER_BINDING, (GLint*)&oldFBO );

	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );
	glClear ( GL_COLOR_BUFFER_BIT );
	fontRenderer.Reset ( fontInfo );

	glBindFramebuffer ( GL_FRAMEBUFFER, oldFBO );

	needUpdateFont = needUpdateImage = GX_FALSE;
}

GXVoid GXHudSurface::AddImage ( const GXImageInfo &imageInfo )
{
	if ( imageInfo.texture->internalFormat != GL_RGBA8 )
	{
		GXLogW ( L"GXHudSurface::AddImage::Error - Поддерживаются только тестуры в формате GL_RGBA8\n" );
		return;
	}

	UpdateFont ();

	GXMat4 imageModGXMat;
	GXMat4 imageModProjGXMat;

	GXFloat halfWidth = imageInfo.insertWidth * 0.5f;
	GXFloat halfHeight = imageInfo.insertHeight * 0.5f;

	GXSetMat4Scale ( imageModGXMat, halfWidth, halfHeight, 1.0f );
	imageModGXMat.m41 = imageInfo.insertX + halfWidth - surfaceWidth * 0.5f;
	imageModGXMat.m42 = imageInfo.insertY + halfHeight - surfaceHeight * 0.5f;
	imageModGXMat.m43 = 1.0f;

	GXMulMat4Mat4 ( imageModProjGXMat, imageModGXMat, imageProjGXMat );

	GLint oldVP[ 4 ];
	glGetIntegerv ( GL_VIEWPORT, oldVP );

	GLuint oldFBO;
	glGetIntegerv ( GL_FRAMEBUFFER_BINDING, (GLint*)&oldFBO );

	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );
	glDisable ( GL_CULL_FACE );
	glDisable ( GL_DEPTH_TEST );

	glViewport ( 0, 0, surfaceWidth, surfaceHeight );

	GLenum buffer = GL_COLOR_ATTACHMENT0;
	glDrawBuffers ( 1, &buffer );

	glUseProgram ( mat_info.shaders[ 0 ].uiId );

	glActiveTexture ( GL_TEXTURE0 );
	glBindTexture ( GL_TEXTURE_2D, imageInfo.textureID );

	glUniformMatrix4fv ( mod_view_proj_matLocation, 1, GL_FALSE, imageModProjGXMat.A );

	GLboolean isBlend;
	glGetBooleanv ( GL_BLEND, &isBlend );

	switch ( imageInfo.overlayType )
	{
		case GX_ALPHA_TRANSPARENCY:
		{
			glEnable ( GL_BLEND );
			glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

			glBindVertexArray ( vaoInfo.vao );
			glDrawArrays ( GL_TRIANGLES, 0, vaoInfo.numVertices );
			glBindVertexArray ( 0 );
			
			if ( !isBlend )
				glDisable ( GL_BLEND );
		}
		break;

		case GX_ALPHA_ADD:
		{
			glEnable ( GL_BLEND );
			glBlendFunc ( GL_ONE, GL_ONE );

			glBindVertexArray ( vaoInfo.vao );
			glDrawArrays ( GL_TRIANGLES, 0, vaoInfo.numVertices );
			glBindVertexArray ( 0 );

			if ( !isBlend )
				glDisable ( GL_BLEND );
		}
		break;

		case GX_ALPHA_MULTIPLY:
		{
			glEnable ( GL_BLEND );
			glBlendFunc ( GL_SRC_ALPHA, GL_ONE );

			glBindVertexArray ( vaoInfo.vao );
			glDrawArrays ( GL_TRIANGLES, 0, vaoInfo.numVertices );
			glBindVertexArray ( 0 );

			if ( !isBlend )
				glDisable ( GL_BLEND );
		}
		break;

		case GX_SIMPLE_REPLACE:
		default:
		{
			glDisable ( GL_BLEND );

			glBindVertexArray ( vaoInfo.vao );
			glDrawArrays ( GL_TRIANGLES, 0, vaoInfo.numVertices );
			glBindVertexArray ( 0 );

			if ( isBlend )
				glEnable ( GL_BLEND );
		}
		break;
	}

	glUseProgram ( 0 );
	glBindFramebuffer ( GL_FRAMEBUFFER, oldFBO );
	glViewport ( oldVP[ 0 ], oldVP[ 1 ], oldVP[ 2 ], oldVP[ 3 ] );

	glEnable ( GL_CULL_FACE );
	glEnable ( GL_DEPTH_TEST );

	needUpdateImage = GX_TRUE;

	fontRenderer.AddImage ( *fontInfo.texture, fontInfo.textureID );
}

GXInt GXHudSurface::AddText ( const GXPenInfo &penInfo, const GXWChar* fmt, ... )
{
	UpdateImage ();
	
	fontInfo.font = penInfo.font;
	fontInfo.fontColor = penInfo.fontColor;
	fontInfo.fontSize = penInfo.fontSize;
	fontInfo.fontAspect = invAspect;
	fontInfo.insertX = penInfo.insertX;
	fontInfo.insertY = penInfo.insertY;
	fontInfo.kerning = penInfo.kerning;

	va_list ap;
	va_start ( ap, fmt );
	GXInt ans = fontRenderer.AddText ( fontInfo, fmt, ap );
	va_end ( ap );

	needUpdateFont = GX_TRUE;

	return ans;
}

GXUInt GXHudSurface::GetTextRasterLength ( const FT_Face font, GXUInt fontSize, GXBool kerning, const GXWChar* fmt, ... )
{
	va_list ap;
	va_start ( ap, fmt );
	GXInt ans = fontRenderer.GetTextRasterLength ( font, fontSize, kerning, fmt, ap );
	va_end ( ap );

	return ans;
}

GXVoid GXHudSurface::SetScale ( GXFloat x, GXFloat y, GXFloat z )
{
	GXMesh::SetScale ( x, y, z );
}

GXVoid GXHudSurface::SetScale ( GXFloat scaleFactor )
{
	GXVec3 old;
	GetScale ( old );

	GXMesh::SetScale ( scaleFactor, scaleFactor * invAspect, 1.0f );
}

GXVoid GXHudSurface::Draw ()
{
	UpdateImage ();
	UpdateFont ();

	const GXMat4* view_proj_mat = gx_ActiveCamera->GetViewProjectionMatrixPtr ();
	GXMat4 mod_view_proj_mat;
	GXMulMat4Mat4 ( mod_view_proj_mat, mod_mat, *view_proj_mat );

	glEnable ( GL_BLEND );
	glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	glUseProgram ( mat_info.shaders[ 0 ].uiId );

	glUniformMatrix4fv ( mod_view_proj_matLocation, 1, GL_FALSE, mod_view_proj_mat.A );

	glActiveTexture ( GL_TEXTURE0 );
	glBindTexture ( GL_TEXTURE_2D, fontInfo.textureID );

	glBindVertexArray ( vaoInfo.vao );
	glDrawArrays ( GL_TRIANGLES, 0, vaoInfo.numVertices );
	glBindVertexArray ( 0 );

	glBindTexture ( GL_TEXTURE_2D, 0 );

	glUseProgram ( 0 );

	glDisable ( GL_BLEND );
}

GXVoid GXHudSurface::Load3DModel ()
{
	GXGetVAOFromOBJ ( vaoInfo, L"../3D Models/System/ScreenQuad.obj", L"../3D Models/System/Cache/ScreenQuad.cache" );
	GXLoadMTR ( L"../Materials/System/Hud_Surface.mtr", mat_info );

	GXGetShaderProgram ( mat_info.shaders[ 0 ] );
}

GXVoid GXHudSurface::InitUniforms ()
{
	glUseProgram ( mat_info.shaders[ 0 ].uiId );

	glUniform1i ( GXGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "texture" ), 0 );
	mod_view_proj_matLocation = GXGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "mod_view_proj_mat" );

	glUseProgram ( 0 );
}

GXVoid GXHudSurface::UpdateFont ()
{
	if ( !needUpdateFont ) return;
	
	fontRenderer.Render ( fontInfo );
	needUpdateFont = GX_FALSE;
}

GXVoid GXHudSurface::UpdateImage ()
{
	if ( !needUpdateImage ) return;
	
	fontRenderer.AddImage ( *fontInfo.texture, fontInfo.textureID );
	needUpdateImage = GX_FALSE;
}