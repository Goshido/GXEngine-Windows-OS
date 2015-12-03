//version 1.9

#include <Test/GXMLPRenderQuad.h>
#include <GXEngine/GXGlobals.h>
#include <GXEngine/GXTextureUtils.h>


static const GLenum gx_mlprqBuffers[ 3 ] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };

GXMLPRenderQuad::GXMLPRenderQuad ()
{
	Load3DModel ();
	InitUniforms ();

	GXGLTextureStruct texture_info;
	texture_info.data = 0;
	texture_info.format = GL_RGBA;
	texture_info.internalFormat = GL_RGBA8;
	texture_info.width = gx_EngineSettings.potWidth;
	texture_info.height = gx_EngineSettings.potHeight;
	texture_info.wrap = GL_CLAMP_TO_EDGE;
	texture_info.type = GL_UNSIGNED_BYTE;

	preoutTexture = GXCreateTexture ( texture_info );
	diffuseTexture = GXCreateTexture ( texture_info );
	figurationTexture = GXCreateTexture ( texture_info );

	texture_info.format = GL_DEPTH_COMPONENT;
	texture_info.internalFormat = GL_DEPTH_COMPONENT32F;
	depthTexture = GXCreateTexture ( texture_info );

	glGenFramebuffers ( 1, &fbo );
	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );
	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, diffuseTexture, 0 );
	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, figurationTexture, 0 );
	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, preoutTexture, 0 );
	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,  GL_TEXTURE_2D, depthTexture, 0 );
	glDrawBuffers ( 3, gx_mlprqBuffers );
	GLenum fboStatus = glCheckFramebufferStatus ( GL_FRAMEBUFFER );
	if ( fboStatus != GL_FRAMEBUFFER_COMPLETE )
	{
		GXLogW ( L"GXMLPRenderQuad::GXMLPRenderQuad::Error - Что-то с FBO мультяшного MLP эффекта. Код ошибки 0x%04x\n", fboStatus );
		GXDebugBox ( L"Что-то с FBO мультяшного MLP эффекта" );
	}

	glBindFramebuffer ( GL_FRAMEBUFFER, 0 );
}

GXMLPRenderQuad::~GXMLPRenderQuad ()
{
	GXRemoveVAO ( vaoInfo );
	GXRemoveShaderProgram ( mat_info.shaders [ 0 ] );
	GXRemoveShaderProgram ( mat_info.shaders [ 1 ] );

	glBindTexture ( GL_TEXTURE_2D, 0 );

	glDeleteTextures ( 1, &preoutTexture );
	glDeleteTextures ( 1, &diffuseTexture );
	glDeleteTextures ( 1, &depthTexture );
	glDeleteTextures ( 1, &figurationTexture );

	glBindFramebuffer ( GL_FRAMEBUFFER, 0 );
	glDeleteFramebuffers ( 1, &fbo );
}

GXVoid GXMLPRenderQuad::Draw ()
{
	const GXMat4* proj_mat = gx_ActiveCamera->GetProjectionMatrixPtr ();
	GXVec4 projParams ( proj_mat->m11, proj_mat->m22, proj_mat->m33, proj_mat->m34 );

	glDepthMask ( GL_FALSE );
	glDrawBuffers ( 1, gx_mlprqBuffers );

	glUseProgram ( mat_info.shaders[ 0 ].uiId );

	glActiveTexture ( GL_TEXTURE0 );
	glBindTexture ( GL_TEXTURE_2D, depthTexture );
	glActiveTexture ( GL_TEXTURE1 );
	glBindTexture ( GL_TEXTURE_2D, preoutTexture );

	glUniform3fv ( dofParamsLocation, 1, dofParams.v );
	glUniform4fv ( projParamsLocation, 1, projParams.v );

	glBindVertexArray ( vaoInfo.vao );
	glDrawArrays ( GL_TRIANGLES, 0, vaoInfo.numVertices );

	glUseProgram ( 0 );
	glBindVertexArray ( 0 );
	glBindTexture ( GL_TEXTURE_2D, 0 );
	glDepthMask ( GL_TRUE );
}

GXVoid GXMLPRenderQuad::CombineLayers ()
{
	const GXMat4* proj_mat = gx_ActiveCamera->GetProjectionMatrixPtr ();
	GXVec4 projParams ( proj_mat->m11, proj_mat->m22, proj_mat->m33, proj_mat->m34 );

	glDepthMask ( GL_FALSE );
	glDrawBuffers ( 1, gx_mlprqBuffers + 2 );

	glUseProgram ( mat_info.shaders[ 1 ].uiId );

	glActiveTexture ( GL_TEXTURE0 );
	glBindTexture ( GL_TEXTURE_2D, depthTexture );
	glActiveTexture ( GL_TEXTURE1 );
	glBindTexture ( GL_TEXTURE_2D, diffuseTexture );
	glActiveTexture ( GL_TEXTURE2 );
	glBindTexture ( GL_TEXTURE_2D, figurationTexture );

	glUniform4fv ( projParamsCombineLayersLocation, 1, projParams.v );

	glBindVertexArray ( vaoInfo.vao );
	glDrawArrays ( GL_TRIANGLES, 0, vaoInfo.numVertices );

	glUseProgram ( 0 );
	glBindVertexArray ( 0 );
	glBindTexture ( GL_TEXTURE_2D, 0 );
	glDepthMask ( GL_TRUE );
}

GLuint GXMLPRenderQuad::GetColorTexture ()
{
	return preoutTexture;
}

GLuint GXMLPRenderQuad::GetDepthTexture ()
{
	return depthTexture;
}

GLuint GXMLPRenderQuad::GetDiffuseTexture ()
{
	return diffuseTexture;
}

GLuint GXMLPRenderQuad::GetFigurationTexture ()
{
	return figurationTexture;
}

GLuint GXMLPRenderQuad::GetFBO ()
{
	return fbo;
}

GXVoid GXMLPRenderQuad::StartFigurationPass ()
{
	glDrawBuffers ( 2, gx_mlprqBuffers );
}

GXVoid GXMLPRenderQuad::StartCommonPass ()
{
	glDrawBuffers ( 1, gx_mlprqBuffers );
}

GXVoid GXMLPRenderQuad::StartTransparencyPass ()
{
	glDrawBuffers ( 1, gx_mlprqBuffers + 2 );
}

GXVoid GXMLPRenderQuad::SetClearColor ( const GXVec4 &clearColor )
{
	this->clearColor = clearColor;
}

GXVoid GXMLPRenderQuad::ClearAllBuffers ()
{
	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );
	glDrawBuffers ( 3, gx_mlprqBuffers );
	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glClearColor ( clearColor.r, clearColor.g, clearColor.b, clearColor.a );
}

GXVoid GXMLPRenderQuad::SetDofParams ( const GXVec3 &dofParams )
{
	this->dofParams = dofParams;
}

GXVoid GXMLPRenderQuad::Load3DModel ()
{
	GXGetVAOFromOBJ ( vaoInfo, L"../3D Models/System/ScreenQuad.obj", L"../3D Models/System/Cache/ScreenQuad.cache" );
	
	GXLoadMTR ( L"../Materials/Test/Render_Quad.mtr", mat_info );
	GXGetShaderProgram ( mat_info.shaders[ 0 ] );
	GXGetShaderProgram ( mat_info.shaders[ 1 ] );
}

GXVoid GXMLPRenderQuad::InitUniforms ()
{
	glUseProgram ( mat_info.shaders[ 0 ].uiId );

	glUniform1i ( GXGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "depthTexture" ), 0 );
	glUniform1i ( GXGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "preoutTexture" ), 1 );
	dofParamsLocation = GXGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "dofParams" );
	projParamsLocation = GXGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "projParams" );

	glUseProgram ( mat_info.shaders[ 1 ].uiId );

	glUniform1i ( GXGetUniformLocation ( mat_info.shaders[ 1 ].uiId, "depthTexture" ), 0 );
	glUniform1i ( GXGetUniformLocation ( mat_info.shaders[ 1 ].uiId, "diffuse" ), 1 );
	glUniform1i ( GXGetUniformLocation ( mat_info.shaders[ 1 ].uiId, "figuration" ), 2 );
	projParamsCombineLayersLocation = GXGetUniformLocation ( mat_info.shaders [ 1 ].uiId, "projParams" );

	glUseProgram ( 0 );
}